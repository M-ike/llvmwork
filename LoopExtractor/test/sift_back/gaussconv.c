/*
*  Jan Prokaj
*  January 31, 2005
*/

#include "gaussconv.h"

//Does a gaussian 1st derivative (G') convolution on the passsed in image.
//The resulting image is dynamically allocated and returned
//via a pointer
Image* gaussianD1Convolution2D(Image *original, double sigma) {

	int    p, q;
	int    i, j; //loop counters
	double maskval, sum1, sum2;
	Image  *outImage;
	double **maskX;
	double **maskY;
	double **outpicX; //stores convolution x-result
	double **outpicY; //stores convolution y-result
	int    mr, maskSize;
	int    centY, centX;
	double normalizeFactor;

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

	/* deallocate memory */
	for(i = 0; i < maskSize; ++i) {
		free(maskX[i]);
		free(maskY[i]);
	}
	free(maskX);
	free(maskY);

	for(i = 0; i < original->height; ++i) {
		free(outpicX[i]);
		free(outpicY[i]);
	}
	free(outpicX);
	free(outpicY);

	return outImage;
}

//Does a gaussian convolution on the passsed in image.
//The resulting image is dynamically allocated and returned
//via a pointer
Image* gaussianConvolution2D(Image *original, double sigma) {

	int    p, q;
	int    i, j; //loop counters
	double maskval, sum;
	Image  *outImage;
	double **mask;
	double **outpic; //stores convolution x-result
	int    mr, maskSize;
	int    centY, centX;
	double normalizeFactor;

	mr = (int)(sigma * 3);
	maskSize = 2*mr + 1;
	centY = centX = mr;

	/* initializations */
	mask = (double **) malloc(maskSize*sizeof(double *));
	if(mask == NULL)
		return NULL;
	for(i = 0; i < maskSize; ++i) {
		mask[i] = (double *) malloc(maskSize*sizeof(double));
	}

	outpic = (double **) malloc(original->height*sizeof(double *));
	if(outpic == NULL)
		return NULL;
	for(i = 0; i < original->height; ++i) {
		outpic[i] = (double *) malloc(original->width*sizeof(double));
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
	      maskval = normalizeFactor*exp(-1*((p*p + q*q)/(2*(sigma*sigma))));
              mask[p+centY][q+centX] = maskval;
           }
        }

	/*convolve masks with picture*/
        for(i=mr; i < original->height - mr; i++) {
          for(j=mr; j< original->width - mr; j++) {
             sum = 0.0;
             for (p=-mr;p<=mr;p++) {
                for (q=-mr;q<=mr;q++) {
                   sum += original->pic[i+p][j+q] * mask[p+centY][q+centX];
                }
             }
             outpic[i][j] = sum;
          }
        }

	/*compute strength/magnitude*/
        for(i = 0; i < original->height; i++) {
          for(j = 0; j < original->width; j++) {
	  	outImage->pic[i][j] = 0.0f;
	  	if(i >= mr && i < original->height - mr &&
					j >= mr && j < original->width - mr) {
			outImage->pic[i][j]=(float) outpic[i][j];
		}
           }
        }

	/* deallocate memory */
	for(i = 0; i < maskSize; ++i) {
		free(mask[i]);
	}
	free(mask);

	for(i = 0; i < original->height; ++i) {
		free(outpic[i]);
	}
	free(outpic);

	return outImage;
}

//Does a fast gaussian convolution on the passsed in image by
//using only a 1D kernel and making vertical and horizontal pass.
//The resulting image is dynamically allocated and returned
//via a pointer
Image* gaussianConvolution2DFast(Image *original, double sigma) {

	int    p, q;
	int    i, j;
	double sum;
	Image  *outImage;
	double *mask;
	double **outpicY; //stores convolution y-result
	int    mr, maskSize, maskMiddle;
	double normalizeFactor;
	int    isOut;
	double outBound;

	mr = (int)(sigma * 3);
	maskSize = 2*mr + 1;
	mask = (double *) malloc(maskSize*sizeof(double));
	if(mask == NULL)
		return NULL;
	maskMiddle = maskSize / 2;

	/* allocate */
	outpicY = (double **) malloc(original->height*sizeof(double *));
	if(outpicY == NULL)
		return NULL;
	for(i = 0; i < original->height; ++i) {
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

	/* initialize */
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			outpicY[i][j] = 0.0;
			outImage->pic[i][j] = 0.0f;
		}
	}

	/*generate mask*/
	normalizeFactor = 1.0/(sqrt(2.0*M_PI)*sigma);
	for(i = 0; i < maskSize; ++i) {
		p = i - maskMiddle;
		mask[i] = normalizeFactor*exp(-1*(p*p)/(2.0*sigma*sigma));
	}

	/*convolve 1D -- vertical pass*/
	for(i = 0; i < original->width; ++i) {
		for(j = 0; j < original->height; ++j) {
			sum = 0.0;
			isOut = 0;
			outBound = 0.0;
			for(p = 0; p < maskSize; ++p) {
				q = p - maskMiddle + j;
				if(q < 0 || q >= original->height) {
					isOut = 1;
					outBound += mask[p];
					continue;
				}
				sum += mask[p]*original->pic[q][i];
			}
			if(isOut)
				sum *= 1.0/(1.0-outBound);
			outpicY[j][i] = sum;
		}
	}

	/*convolve 1D -- horizontal pass*/
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			sum = 0.0;
			isOut = 0;
			outBound = 0.0;
			for(p = 0; p < maskSize; ++p) {
				q = p - maskMiddle + j;
				if(q < 0 || q >= original->width) {
					isOut = 1;
					outBound += mask[p];
					continue;
				}
				sum += mask[p]*outpicY[i][q];
			}
			if(isOut)
				sum *= 1.0/(1.0-outBound);
			outImage->pic[i][j] = sum;
		}
	}

	/* deallocate memory */
	free(mask);
	for(i = 0; i < original->height; ++i) {
		free(outpicY[i]);
	}
	free(outpicY);

	return outImage;
}
