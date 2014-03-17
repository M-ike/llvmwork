/*
*  Jan Prokaj
*  May 11, 2005
*/
#include "canny.h"

//basic algorithm, no performance improvements
Image *getCannyEdgesClassic(Image *original, float sigma, float percent) {

	int i, j;
	int    p, q;
	double maskval, sum1, sum2;
	Image  *outImage, *peaks, *canny;
	double **maskX;
	double **maskY;
	double **outpicX; //stores convolution x-result
	double **outpicY; //stores convolution y-result
	int    mr, maskSize;
	int    centY, centX;
	double normalizeFactor;
	float  max;
	float  xDiff, yDiff;
	double angle;
	int   hist[256] = {0};
	int   sum;
	float picPercent, HI, LO;
	
	mr = (int)(sigma * 3);
	maskSize = 2*mr + 1;
	centY = centX = mr;

	/* initializations */
	maskX = (double **) malloc(maskSize*sizeof(double *));
	maskY = (double **) malloc(maskSize*sizeof(double *));
	if(maskX == NULL || maskY == NULL)
		return NULL;
	for(i = 0; i < maskSize; ++i) {
		maskX[i] = (double *) malloc(maskSize*sizeof(double));
		maskY[i] = (double *) malloc(maskSize*sizeof(double));
	}

	outpicX = (double **) malloc(original->height*sizeof(double *));
	outpicY = (double **) malloc(original->height*sizeof(double *));
	if(outpicX == NULL || outpicY == NULL)
		return NULL;
	for(i = 0; i < original->height; ++i) {
		outpicX[i] = (double *) malloc(original->width*sizeof(double));
		outpicY[i] = (double *) malloc(original->width*sizeof(double));
	}

	outImage = (Image *) malloc(sizeof(Image));
	outImage->width = original->width;
	outImage->height = original->height;
	outImage->pic = (float **) malloc(outImage->height*sizeof(float *));
	if(outImage->pic == NULL)
		return NULL;
	for(i = 0; i < outImage->height; ++i) {
		outImage->pic[i] = (float *) malloc(outImage->width*sizeof(float));
	}

	/*generate masks*/
	normalizeFactor = 1.0/(2.0*M_PI*sigma*sigma);
	for(p=-mr;p<=mr;p++) {
		for(q=-mr;q<=mr;q++) {
			maskval = -1*normalizeFactor*(q/(sigma*sigma))*exp(-1*((p*p + q*q)/(2*(sigma*sigma))));
			maskX[p+centY][q+centX] = maskval;
			maskval = -1*normalizeFactor*(p/(sigma*sigma))*exp(-1*((p*p + q*q)/(2*(sigma*sigma))));
			maskY[p+centY][q+centX] = maskval;
		}
	}

	/*convolve masks with picture*/
	for(i=mr; i < original->height - mr; i++) {
		for(j=mr; j< original->width - mr; j++) {
			sum1 = 0.0;
			sum2 = 0.0;
			for (p=-mr;p<=mr;p++) {
				for (q=-mr;q<=mr;q++) {
					sum1 += original->pic[i+p][j+q] * maskX[p+centY][q+centX];
					sum2 += original->pic[i+p][j+q] * maskY[p+centY][q+centX];
				}
			}
			outpicX[i][j] = sum1;
			outpicY[i][j] = sum2;
		}
	}

	/*compute strength/magnitude*/
	for(i = 0; i < original->height; i++) {
		for(j = 0; j < original->width; j++) {
		  	outImage->pic[i][j] = 0.0f;
		  	if(i >= mr && i < original->height - mr &&
						j >= mr && j < original->width - mr) {
				outImage->pic[i][j]=(float) sqrt((outpicX[i][j]*outpicX[i][j]) +
							(outpicY[i][j]*outpicY[i][j]));
			}
		}
	}
		
	if(percent < 0 || percent > 1)
		percent = 0.20;
		
	/* scale to (0,1) for canny -- necessary? */
	max = 0.0;
	for(i = 0; i < outImage->height; ++i) {
		for(j = 0; j < outImage->width; ++j) {
			if(outImage->pic[i][j] > max)
				max = outImage->pic[i][j];
		}
	}
	for(i = 0; i < outImage->height; ++i) {
		for(j = 0; j < outImage->width; ++j) {
			outImage->pic[i][j] /= max;
		}
	}
	
	/* allocate peaks image */
	peaks = (Image *) malloc(sizeof(Image));
	peaks->width = original->width;
	peaks->height = original->height;
	peaks->pic = (float **) malloc(peaks->height*sizeof(float *));
	for(i = 0; i < peaks->height; ++i) {
		peaks->pic[i] = (float *) malloc(peaks->width*sizeof(float));
	}

	/*edge detection/peaks of ridges*/
	for(i = 0; i < peaks->height; ++i) {
		for (j = 0; j < peaks->width; ++j) {
			peaks->pic[i][j] = 0.0;
			if(i == 0 || j == 0 || i == peaks->height - 1 || j == peaks->width - 1) {
				continue;
			}
			xDiff = outpicX[i][j];
			yDiff = outpicY[i][j];
			if(xDiff == 0)
				xDiff = 0.00000001;
			angle = atan(yDiff/xDiff);
			if((angle >= -M_PI / 8.0) && (angle < M_PI / 8.0)) {
				if((outImage->pic[i][j] > outImage->pic[i][j+1]) &&
				    (outImage->pic[i][j] > outImage->pic[i][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else if((angle >= M_PI / 8.0) && (angle < 3.0*M_PI / 8.0)) {
				if((outImage->pic[i][j] > outImage->pic[i+1][j+1]) && (outImage->pic[i][j] > outImage->pic[i-1][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else if((angle >= -3.0*M_PI / 8.0) && (angle < -M_PI / 8.0)) {
				if((outImage->pic[i][j] > outImage->pic[i-1][j+1]) && (outImage->pic[i][j] > outImage->pic[i+1][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else {
				if((outImage->pic[i][j] > outImage->pic[i+1][j]) && (outImage->pic[i][j] > outImage->pic[i-1][j])) {
					peaks->pic[i][j] = 1.0;
				}
			}
		}
	}

	/*compute high/low thresholds*/
	for(i = 0; i < outImage->height; ++i) {
	   for(j = 0; j < outImage->width; ++j) {
	   	//printf("%d ", (int) (outImage->pic[i][j]*255));
	      hist[(int) (outImage->pic[i][j]*255)]++;
	   }
	}
	sum = 0;
	picPercent = (int) (percent*outImage->width*outImage->height);
	for(i = 255; i >= 0; --i) {
	   sum += hist[i];
	   if(sum >= picPercent) {
	      HI = i;
	      break;
	   }
	}
	LO = 0.35*HI;

	/* allocate canny image */
	canny = (Image *) malloc(sizeof(Image));
	canny->width = original->width;
	canny->height = original->height;
	canny->pic = (float **) malloc(canny->height*sizeof(float *));
	for(i = 0; i < canny->height; ++i) {
		canny->pic[i] = (float *) malloc(canny->width*sizeof(float));
	}
	for(i = 0; i < canny->height; ++i) {
		for(j = 0; j < canny->width; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}

	/* hysterisis/double thresholding */
	for(i = 0; i < outImage->height; ++i) {
	   for(j = 0; j < outImage->width; ++j) {
	      if(peaks->pic[i][j] == 1.0) {
	         if(outImage->pic[i][j] > (HI/255)) {
		    peaks->pic[i][j] = 0.0;
		    canny->pic[i][j] = 1.0;
		 }
		 else if(outImage->pic[i][j] < (LO/255)) {
		    peaks->pic[i][j] = 0.0;
		 }
	      }
	   }
	}

	for(i = 1; i < peaks->height - 1; ++i) {
		for(j = 1; j < peaks->width - 1; ++j) {
			if(peaks->pic[i][j] == 1.0) {
				traceRidge(peaks, canny, i, j);
			}
		}
	}
	
	/* clear border pixels */
	for(i = 0; i <= mr; ++i) {
		for(j = 0; j < canny->width; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}
	for(i = canny->height - mr; i < canny->height; ++i) {
		for(j = 0; j < canny->width; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}
	for(i = 0; i < canny->height; ++i) {
		for(j = 0; j <= mr; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}
	for(i = 0; i < canny->height; ++i) {
		for(j = canny->width - mr; j < canny->width; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}

	/* deallocate memory */
	for(i = 0; i < peaks->height; ++i) {
		free(peaks->pic[i]);
		free(outImage->pic[i]);
		//free anglesource stuff
	}
	free(peaks->pic);
	free(outImage->pic);
	free(peaks);
	free(outImage);

	return canny;
}

//runs canny edge-finding algorithm
//angleSource can be NULL, if it is, we have regular canny (CURRENTLY BROKEN)
Image *getCannyEdges(Image *image, float sigma, float percent, Image *angleSource) {

	int   i,j;
	float max;
	Image *peaks;
	Image *canny;
	float xDiff, yDiff, slope;
	double angle;
	int   hist[256] = {0};
	int   sum;
	float picPercent, HI, LO;
	Image *original; //temp
	float **xDeriv, **yDeriv;

	original = cloneImage(image); //we don't want to touch the passed in image
	if(!original) {
		printf("***canny.c/getCannyEdges: Cloning an image failed.\n");
		return NULL;
	}
	if(angleSource == NULL) {
		/* convolution */
		original = gaussianConvolution2DFast(original, sigma);
		if(!original) {
			printf("***canny.c/getCannyEdges: Convolution failed.\n");
			return NULL;
		}
		xDeriv = getDerivativeX(original);
		yDeriv = getDerivativeY(original);
		
		/* compute gradient */
		for(i = 0; i < original->height; ++i) {
			for(j = 0; j < original->width; ++j) {
				original->pic[i][j] = (float) sqrt(xDeriv[i][j]*xDeriv[i][j] + yDeriv[i][j]*yDeriv[i][j]);
			}
		}
	}
	
	if(percent < 0 || percent > 1)
		percent = 0.20;
		
	/* scale to (0,1) for canny -- necessary? */
	max = 0.0;
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			if(original->pic[i][j] > max)
				max = original->pic[i][j];
		}
	}
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			original->pic[i][j] /= max;
		}
	}
	
	/* allocate peaks image */
	peaks = (Image *) malloc(sizeof(Image));
	peaks->width = original->width;
	peaks->height = original->height;
	peaks->pic = (float **) malloc(peaks->height*sizeof(float *));
	for(i = 0; i < peaks->height; ++i) {
		peaks->pic[i] = (float *) malloc(peaks->width*sizeof(float));
	}

	/*edge detection/peaks of ridges*/
	for(i = 0; i < peaks->height; ++i) {
		for (j = 0; j < peaks->width; ++j) {
			peaks->pic[i][j] = 0.0;
			if(i == 0 || j == 0 || i == peaks->height - 1 || j == peaks->width - 1) {
				continue;
			}
			if(angleSource == NULL) {
				xDiff = xDeriv[i][j];
				yDiff = yDeriv[i][j];
			}
			else {
				xDiff = angleSource->pic[i][j+1] - angleSource->pic[i][j-1];
				yDiff = angleSource->pic[i-1][j] - angleSource->pic[i+1][j];
			}
			if(xDiff == 0)
				xDiff = 0.00000001;
			angle = atan(yDiff/xDiff);
			if((angle >= -M_PI / 8.0) && (angle < M_PI / 8.0)) {
				if((original->pic[i][j] > original->pic[i][j+1]) &&
				    (original->pic[i][j] > original->pic[i][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else if((angle >= M_PI / 8.0) && (angle < 3.0*M_PI / 8.0)) {
				if((original->pic[i][j] > original->pic[i-1][j+1]) && (original->pic[i][j] > original->pic[i+1][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else if((angle >= -3.0*M_PI / 8.0) && (angle < -M_PI / 8.0)) {
				if((original->pic[i][j] > original->pic[i+1][j+1]) && (original->pic[i][j] > original->pic[i-1][j-1])) {
					peaks->pic[i][j] = 1.0;
				}
			}
			else {
				if((original->pic[i][j] > original->pic[i+1][j]) && (original->pic[i][j] > original->pic[i-1][j])) {
					peaks->pic[i][j] = 1.0;
				}
			}
		}
	}

	/*compute high/low thresholds*/
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			//printf("%d ", (int) (original->pic[i][j]*255));
			hist[(int) (original->pic[i][j]*255)]++;
		}
	}
	sum = 0;
	picPercent = (int) (percent*original->width*original->height);
	for(i = 255; i >= 0; --i) {
		sum += hist[i];
		if(sum >= picPercent) {
			HI = i;
			break;
		}
	}
	LO = 0.35*HI;

	/* allocate canny image */
	canny = (Image *) malloc(sizeof(Image));
	canny->width = original->width;
	canny->height = original->height;
	canny->pic = (float **) malloc(canny->height*sizeof(float *));
	for(i = 0; i < canny->height; ++i) {
		canny->pic[i] = (float *) malloc(canny->width*sizeof(float));
	}
	for(i = 0; i < canny->height; ++i) {
		for(j = 0; j < canny->width; ++j) {
			canny->pic[i][j] = 0.0;
		}
	}

	/* hysterisis/double thresholding */
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			if(peaks->pic[i][j] == 1.0) {
				if(original->pic[i][j] > (HI/255)) {
					peaks->pic[i][j] = 0.0;
					canny->pic[i][j] = 1.0;
				}
				else if(original->pic[i][j] < (LO/255)) {
					peaks->pic[i][j] = 0.0;
				}
			}
		}
	}

	for(i = 1; i < peaks->height - 1; ++i) {
		for(j = 1; j < peaks->width - 1; ++j) {
			if(peaks->pic[i][j] == 1.0) {
				traceRidge(peaks, canny, i, j);
			}
		}
	}
	
	/* deallocate memory */
	for(i = 0; i < peaks->height; ++i) {
		free(peaks->pic[i]);
		free(original->pic[i]);
		//free anglesource stuff
	}
	free(peaks->pic);
	free(original->pic);
	free(peaks);
	free(original);
	//free anglesource stuff

	return canny;
}

void traceRidge(Image *peaks, Image *canny, int i, int j) {
	int p, q;
	if(i < 1 || i > peaks->height - 1 || j < 1 || j > peaks->width - 1)
		return;
	for(p = -1; p <= 1; ++p) {
		for(q = -1; q <= 1; ++q) {
			if(canny->pic[i+p][j+q] == 1.0) {
				peaks->pic[i][j] = 0.0;
				canny->pic[i][j] = 1.0;
				if(peaks->pic[i+p][j+q] == 1.0) {
					traceRidge(peaks, canny, i+p, j+q);
				}
			}
		}
	}
}

/************************************************
   Jason's Canny code ported from Java
      -- needed for a precise port of HeadFinder
*************************************************/
Canny *newCannyPGM(Image *image, double sigma) {

	ImageRGB *imageRGB;
	Canny *new;
	
	imageRGB = convertToRGB(image);
	new = newCannyPPM(imageRGB, sigma);
	freeImageRGB(imageRGB);
	return new;
}

Canny *newCannyPPM(ImageRGB *image, double sigma) {

	Canny  *new;
	int    i, j, p, q, y, x, gSize, border;
	double **gaussianx, **gaussiany;
	double kappa;
	
	double dgxSum = 0, dgySum = 0; //for convolution
	double xrSum = 0, yrSum = 0; //for convolution
	double xgSum = 0, ygSum = 0; //for convolution
	double xbSum = 0, ybSum = 0; //for convolution
	double rMag = 0.0, gMag = 0.0, bMag = 0.0; //for convolution
	double max; //for convolution
	
	int    **peaks, sector; //for nonmaxima suppresion
	double low, high; //for thresholding
	int    **traced; //for hysterisis
	
	Image *debug;
	
	new = (Canny *) malloc(sizeof(Canny));
	new->width = image->width;
	new->height = image->height;
	new->sigma = sigma;

	/* calculate mask size */
	gSize = (int) ((sigma*6)+1+0.5);
        if(gSize % 2 == 0)
		gSize--;
		
	/* allocate the mask*/
	gaussianx = (double **) malloc(gSize*sizeof(double *));
	for(i = 0; i < gSize; ++i) {
		gaussianx[i] = (double *) malloc(gSize*sizeof(double));
	}
        gaussiany = (double **) malloc(gSize*sizeof(double *));
	for(i = 0; i < gSize; ++i) {
		gaussiany[i] = (double *) malloc(gSize*sizeof(double));
	}
        
	/* constant */
	kappa = 1 / sqrt(2*M_PI*sigma*sigma);
        
	/* calculate mask */
	//q and p switched
	for(i = -gSize/2; i < gSize/2 + 1; ++i) {
		q = i + gSize/2;
		for(j = -gSize/2; j < gSize/2 + 1; ++j) {
			p = j + gSize/2;
                
			gaussianx[q][p] = (-kappa*j/(sigma*sigma))*exp((-j*j - i*i)/(2*sigma*sigma));
			gaussiany[q][p] = (-kappa*i/(sigma*sigma))*exp((-j*j - i*i)/(2*sigma*sigma));
		}
	}
	
	/* allocate magnitude and theta arrays */
	new->magnitude = (double **) malloc(new->height*sizeof(double *));
	for(i = 0; i < new->height; ++i) {
		new->magnitude[i] = (double *) malloc(new->width*sizeof(double));
	}
	new->theta = (double **) malloc(new->height*sizeof(double *));
	for(i = 0; i < new->height; ++i) {
		new->theta[i] = (double *) malloc(new->width*sizeof(double));
	}
	
	/* convolve */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			dgxSum = 0.0;
			dgySum = 0.0;
			xrSum = 0.0;
			yrSum = 0.0;
			xgSum = 0.0;
			ygSum = 0.0;
			xbSum = 0.0;
			ybSum = 0.0;
                
			for(i = -gSize/2; i < gSize/2 + 1; ++i) {
				q = i + gSize/2;
				for(j = -gSize/2; j < gSize/2 + 1; ++j) {
					p = j + gSize/2;
					if(x+j >= 0 && 
					   x+j < new->width && 
					   y+i >= 0 && 
					   y+i < new->height) {
						xrSum += image->red->pic[y+i][x+j] * gaussianx[q][p];
						yrSum += image->red->pic[y+i][x+j] * gaussiany[q][p];
						xgSum += image->green->pic[y+i][x+j] * gaussianx[q][p];
						ygSum += image->green->pic[y+i][x+j] * gaussiany[q][p];
						xbSum += image->blue->pic[y+i][x+j] * gaussianx[q][p];
						ybSum += image->blue->pic[y+i][x+j] * gaussiany[q][p];
						dgxSum += gaussianx[q][p];
						dgySum += gaussiany[q][p];
					}
				}
			}
                
			xrSum = xrSum / dgxSum;
			yrSum = yrSum / dgySum;
			xgSum = xgSum / dgxSum;
			ygSum = ygSum / dgySum;
			xbSum = xbSum / dgxSum;
			ybSum = ybSum / dgySum;
			
			rMag = sqrt(xrSum*xrSum + yrSum*yrSum);
			gMag = sqrt(xgSum*xgSum + ygSum*ygSum);
			bMag = sqrt(xbSum*xbSum + ybSum*ybSum);
			
			new->magnitude[y][x] = max3(rMag, gMag, bMag);
			if(new->magnitude[y][x] == rMag)
				new->theta[y][x] = atan2(yrSum, xrSum);
			else if(new->magnitude[y][x] == gMag)
				new->theta[y][x] = atan2(ygSum, xgSum);
			else
				new->theta[y][x] = atan2(ybSum, xbSum);
		}
        }
	
	max = 0.0;
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			if(new->magnitude[i][j] > max)
				max = new->magnitude[i][j];
		}
	}
	for(i = 0; i < new->height; i++){
		for(j = 0; j < new->width; j++){
			new->magnitude[i][j] = new->magnitude[i][j] / max;
		}
	}
	
	/* deallocate gaussianx, gaussiany */
	for(i = 0; i < gSize; ++i) {
		free(gaussianx[i]);
		free(gaussiany[i]);
	}
        free(gaussianx);
	free(gaussiany);
	
	/* nonmaxima suppresion */
	/* allocate peaks array */
	peaks = (int **) malloc(new->height*sizeof(int *));
	for(i = 0; i < new->height; ++i) {
		peaks[i] = (int *) malloc(new->width*sizeof(int));
	}
	// iterate through the gradient image
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			peaks[y][x] = 0;
                
			/* boundary condition */
			if(y < 1 || y >= new->height - 1 ||
			   x < 1 || x >= new->width - 1)
			   	continue;
			
			// get the "sector" of the pixel (this corresponds to its gradient's direction
			sector = sectorAngle(new, x, y);
                
			// for each sector, check the adjacent pixels
			// horizontal case
			if(sector == 0) {
				if(new->magnitude[y][x] > new->magnitude[y][x-1]) {
					if(new->magnitude[y][x] > new->magnitude[y][x+1])
						peaks[y][x] = 255;
                        	} 
				else if (new->magnitude[y][x] > new->magnitude[y][x+1]) {
					if(new->magnitude[y][x] == new->magnitude[y][x-1]) {
						peaks[y][x] = 255;
						new->magnitude[y][x+1] -= 0.0001;
					}
				}
			}
			
			// bottom to top diagonal case
			if(sector == 1){
				//if(new->magnitude[x][y] > new->magnitude[x+1][y+1] && new->magnitude[x][y] > new->magnitude[x-1][y-1])
				if(new->magnitude[y][x] > new->magnitude[y-1][x+1]) {
					if(new->magnitude[y][x] > new->magnitude[y+1][x-1])
						peaks[y][x] = 255;
				}
				else if(new->magnitude[y][x] > new->magnitude[y+1][x-1]) {
					if(new->magnitude[y][x] == new->magnitude[y-1][x+1]) {
						peaks[y][x] = 255;
						new->magnitude[y-1][x+1] -= 0.0001;
					}
				}
			}
			
			// vertical case
			if(sector == 2) {
				if(new->magnitude[y][x] > new->magnitude[y+1][x]) {
					if(new->magnitude[y][x] > new->magnitude[y-1][x]) {
						peaks[y][x] = 255;
					}
				}
				else if(new->magnitude[y][x] > new->magnitude[y-1][x]) {
					if(new->magnitude[y][x] == new->magnitude[y+1][x]) {
						peaks[y][x] = 255;
						new->magnitude[y+1][x] -= 0.0001;
					}
				}
			}
			
			// top to bottom diagonal case
			if(sector == 3){
				//if(new->magnitude[x][y] > new->magnitude[x+1][y-1] && new->magnitude[x][y] > new->magnitude[x-1][y+1])
				if(new->magnitude[y][x] > new->magnitude[y+1][x+1]) {
					if(new->magnitude[y][x] > new->magnitude[y-1][x-1])
						peaks[y][x] = 255;
				}
				else if(new->magnitude[y][x] > new->magnitude[y-1][x-1]) {
					if(new->magnitude[y][x] == new->magnitude[y+1][x+1]) {
						peaks[y][x] = 255;
						new->magnitude[y+1][x+1] -= 0.0001;
					}
				}
			}
		}
	}
	
	debug = allocateImageFromDoubles(new->magnitude, new->width, new->height);
	writePGMFile(debug, "magnitudes.pgm");
	
	debug = allocateImageFromInts(peaks, 255, new->width, new->height);
	writePGMFile(debug, "peaks.pgm");
		
	findThresholds(new, &low, &high);
	
	/* eliminate low values */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(peaks[y][x] == 255 && new->magnitude[y][x] < low)
				new->magnitude[y][x] = 0.0;
		}
        }
	
	new->edges = allocateImage(new->width, new->height);
        
	/* keep high values */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(peaks[y][x] == 255){
				if(new->magnitude[y][x] >= high) {
					new->edges->pic[y][x] = 1.0;
					//magnitude[x][y] = 0;
				}
				else {
					new->edges->pic[y][x] = 0.0;
				}
			}
		}
	}
	
	writePGMFile(new->edges, "canny_thresh.pgm");
	
	/* allocate and initialize traced array */
	traced = (int **) malloc(new->height*sizeof(int *));
	for(i = 0; i < new->height; ++i) {
		traced[i] = (int *) malloc(new->width*sizeof(int));
	}
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			traced[i][j] = 0;
		}
	}
        
        // iterate through the pixels, if we find an edge that hasn't been traced yet, start
        //   the recursion there
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(new->edges->pic[y][x] == 1.0 && !traced[y][x])
				traceColor(new, x, y, low, traced, peaks);
		}
	}
	
	/* clear border pixels, assumes picture is at least border x border */
	border = (int) (3*new->sigma);
	for(j = 0; j <= border; ++j) {
		for(i = 0; i < new->height; ++i) {
			new->edges->pic[i][j] = 0.0;
		}
	}
	for(j = new->width - border; j < new->width; ++j) {
		for(i = 0; i < new->height; ++i) {
			new->edges->pic[i][j] = 0.0;
		}
	}
	for(i = 0; i <= border; ++i) {
		for(j = 0; j < new->width; ++j) {
			new->edges->pic[i][j] = 0.0;
		}
	}
	for(i = new->height - border; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			new->edges->pic[i][j] = 0.0;
		}
	}
	
	/* deallocate peaks and traced arrays */
	for(i = 0; i < new->height; ++i) {
		free(peaks[i]);
		free(traced[i]);
	}
	free(peaks);
	free(traced);
	
	return new;
}

Canny  *newCannyPGMTwoSources(Image *gradient, Image *gaussian) {

	Canny  *new;
	int    i, j, p, q, y, x;
	int    **peaks, sector; //for nonmaxima suppresion
	double low, high; //for thresholding
	int    **traced; //for hysterisis
	
	new = (Canny *) malloc(sizeof(Canny));
	new->width = gradient->width;
	new->height = gradient->height;
	new->sigma = -1.0; //unknown
	
	/* allocate magnitude and theta arrays */
	new->magnitude = (double **) malloc(new->height*sizeof(double *));
	for(i = 0; i < new->height; ++i) {
		new->magnitude[i] = (double *) malloc(new->width*sizeof(double));
	}
	new->theta = (double **) malloc(new->height*sizeof(double *));
	for(i = 0; i < new->height; ++i) {
		new->theta[i] = (double *) malloc(new->width*sizeof(double));
	}
	
	/* initialize magnitudes and thetas */
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			new->magnitude[i][j] = gradient->pic[i][j];
			if(j > 0 && j < new->width - 1 && i > 0 && i < new->height - 1) {
				new->theta[i][j] = atan2(gaussian->pic[i-1][j] - gaussian->pic[i+1][j], gaussian->pic[i][j+1] - gaussian->pic[i][j-1]);
			}
		}
	}
	
	/* nonmaxima suppresion */
	/* allocate peaks array */
	peaks = (int **) malloc(new->height*sizeof(int *));
	for(i = 0; i < new->height; ++i) {
		peaks[i] = (int *) malloc(new->width*sizeof(int));
	}
	
	/* find peaks */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			peaks[y][x] = 0;
			
			/* boundary condition */
			if(y < 1 || y >= new->height - 1 ||
			   x < 1 || x >= new->width - 1)
			   	continue;
                
			// get the "sector" of the pixel (this corresponds to its gradient's direction
			sector = sectorAngle(new, x, y);
                
			// for each sector, check the adjacent pixels
			// horizontal case
			if(sector == 0) {
				if(new->magnitude[y][x] > new->magnitude[y][x-1]) {
					if(new->magnitude[y][x] > new->magnitude[y][x+1])
						peaks[y][x] = 255;
                        	} 
				else if (new->magnitude[y][x] > new->magnitude[y][x+1]) {
					if(new->magnitude[y][x] == new->magnitude[y][x-1]) {
						peaks[y][x] = 255;
						new->magnitude[y][x+1] -= 0.0001;
					}
				}
			}
			
			// bottom to top diagonal case
			if(sector == 1){
				//if(new->magnitude[x][y] > new->magnitude[x+1][y+1] && new->magnitude[x][y] > new->magnitude[x-1][y-1])
				if(new->magnitude[y][x] > new->magnitude[y-1][x+1]) {
					if(new->magnitude[y][x] > new->magnitude[y+1][x-1])
						peaks[y][x] = 255;
				}
				else if(new->magnitude[y][x] > new->magnitude[y+1][x-1]) {
					if(new->magnitude[y][x] == new->magnitude[y-1][x+1]) {
						peaks[y][x] = 255;
						new->magnitude[y-1][x+1] -= 0.0001;
					}
				}
			}
			
			// vertical case
			if(sector == 2){
				if(new->magnitude[y][x] > new->magnitude[y+1][x]) {
					if(new->magnitude[y][x] > new->magnitude[y-1][x])
						peaks[y][x] = 255;
				}
				else if(new->magnitude[y][x] > new->magnitude[y-1][x]) {
					if(new->magnitude[y][x] == new->magnitude[y+1][x]) {
						peaks[y][x] = 255;
						new->magnitude[y+1][x] -= 0.0001;
					}
				}
			}
			
			// top to bottom diagonal case
			if(sector == 3){
				//if(new->magnitude[x][y] > new->magnitude[x+1][y-1] && new->magnitude[x][y] > new->magnitude[x-1][y+1])
				if(new->magnitude[y][x] > new->magnitude[y+1][x+1]) {
					if(new->magnitude[y][x] > new->magnitude[y-1][x-1])
						peaks[y][x] = 255;
				}
				else if(new->magnitude[y][x] > new->magnitude[y-1][x-1]) {
					if(new->magnitude[y][x] == new->magnitude[y+1][x+1]) {
						peaks[y][x] = 255;
						new->magnitude[y+1][x+1] -= 0.0001;
					}
				}
			}
		}
	}
	
	findThresholds(new, &low, &high);
	
	/* eliminate low values */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(peaks[y][x] == 255 && new->magnitude[y][x] < low)
				new->magnitude[y][x] = 0.0;
		}
        }
	
	new->edges = allocateImage(new->width, new->height);
        
	/* keep high values */
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(peaks[y][x] == 255){
				if(new->magnitude[y][x] >= high) {
					new->edges->pic[y][x] = 1.0;
					//magnitude[x][y] = 0;
				}
				else {
					new->edges->pic[y][x] = 0.0;
				}
			}
		}
	}
	
	/* allocate and initialize traced array */
	traced = (int **) malloc(new->height*sizeof(int *));
	for(i = 0; i < new->height; ++i) {
		traced[i] = (int *) malloc(new->width*sizeof(int));
	}
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			traced[i][j] = 0;
		}
	}
        
        // iterate through the pixels, if we find an edge that hasn't been traced yet, start
        //   the recursion there
	for(y = 0; y < new->height; ++y) {
		for(x = 0; x < new->width; ++x) {
			if(new->edges->pic[y][x] == 1.0 && !traced[y][x])
				traceColor(new, x, y, low, traced, peaks);
		}
	}
	
	/* deallocate peaks and traced arrays */
	for(i = 0; i < new->height; ++i) {
		free(peaks[i]);
		free(traced[i]);
	}
	free(peaks);
	free(traced);
	
	return new;
}

//you don't need doubles here, floats will be good
void findThresholds(Canny *this, double *low, double *high) {
	//thresholding
	int area = 0;                 // histogram area
	int hist[256] = {0};   // histogram
	int c;                      // the point in the histogram where "area" pixels of the image are
                                    //  to the right
	double percent = 0.05;      // the percent of pixels in the image to choose,was 0.05
        int i, j;
        
	
	
	// fill the histogram
	for (i = 0; i < this->height; ++i) {
		for (j = 0; j < this->width; ++j) {
			hist[(int)(this->magnitude[i][j]*255)]++;
		}
	}
        
        // work your way down the  histogram from the right, adding up the area
        //   if the area you're at is greater than the percent of pixels you want to choose,
        //   finish
	for(c = 255; c >= 0; c--){
		area = area + hist[c];
		if ((percent*this->width*this->height) < area){
			break;
		}
	}
        
	// the value of C that you stop at is the high threshold
	*high = (double) c / 255; //we are using floats
	*low = *high * 0.2;  // set the low threshold to 20% of the high threshold
}

//straight from canny.java
void traceColor(Canny *this, int x, int y, double lowThreshold, int **traced, int **peaks) {

	int i, j;

	// stopping case - if we're out of bounds
	if(x < 1 || x >= this->width - 1 || y < 1 || y >= this->height - 1)
		return;
        
	// stopping case - if we've already been here
	if(traced[y][x])
		return;
        
        // stopping case - if the current magnitude is lower than the lowest acceptable value
	if(this->magnitude[y][x] < lowThreshold)
		return;
        
        // mark this pixel as visited
        traced[y][x] = 1;
        
        // mark this pixel as an edge
        this->edges->pic[y][x] = 1.0;
        
        // recursively traverse to the adjacent pixels if they meet the criteria.of an edge
	for(i = -1; i <= 1; ++i) {
		for(j = -1; j <= 1; ++j) {
			if(this->magnitude[y+i][x+j] > lowThreshold && peaks[y+i][x+j] == 255) {
				traceColor(this, x+j, y+i, lowThreshold, traced, peaks);
			}
		}
	}
}

int sectorAngle(Canny *this, int x, int y) {
	//horizontal case
	if(((this->theta[y][x] >= -M_PI/8.0) && (this->theta[y][x] < M_PI/8.0)) || (this->theta[y][x] < -7*M_PI/8.0) || (this->theta[y][x] >= 7*M_PI/8.0)){
		return 0;
	}
	// bottom to top diagonal case
	if(((this->theta[y][x] >= M_PI/8.0) && (this->theta[y][x] < 3*M_PI/8.0)) || ((this->theta[y][x] < -5*M_PI/8.0) && (this->theta[y][x] >= -7*M_PI/8.0))){
		return 1;
	}
	// vertical case
	if(((this->theta[y][x] >= 3*M_PI/8.0) && (this->theta[y][x] < 5*M_PI/8.0)) || ((this->theta[y][x] < -3*M_PI/8.0) && (this->theta[y][x] >= -5*M_PI/8.0))){
		return 2;
	}
	// top to bottom diagonal case
	if(((this->theta[y][x] >= 5*M_PI/8.0) && (this->theta[y][x] < 7*M_PI/8.0)) || ((this->theta[y][x] < -M_PI/8.0) && (this->theta[y][x] >= -3*M_PI/8.0))){
		return 3;
	}
        return -1;
}

double max3(double d1, double d2, double d3) {
	double max = d1;
        if(d2 > max) max = d2;
        if(d3 > max) max = d3;
        return max;
}

//int **getEdges

int isEdge(Canny *this, int x, int y) {
	if(y < 0 || y >= this->height || x <  0 || x >= this->width) {
		printf("***canny.c/isEdge(): Warning, out of bounds.\n");
		return 0;
	}
	if(this->edges->pic[y][x] >= 0.9999)
		return 1;
	return 0;
}

void freeCanny(Canny *this) {

	int i;

	freeImage(this->edges);
	for(i = 0; i < this->height; ++i) {
		free(this->magnitude[i]);
		free(this->theta[i]);
	}
	free(this);
}
