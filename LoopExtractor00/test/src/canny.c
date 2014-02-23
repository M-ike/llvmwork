#include "canny_loop_68.h"
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
	
    getCannyEdgesClassic_loop_1(&i, &maskSize, &maskX, &maskY);


	outpicX = (double **) malloc(original->height*sizeof(double *));
	outpicY = (double **) malloc(original->height*sizeof(double *));
	if(outpicX == NULL || outpicY == NULL)
		return NULL;
	
    getCannyEdgesClassic_loop_2(&i, &original, &outpicX, &outpicY);


	outImage = (Image *) malloc(sizeof(Image));
	outImage->width = original->width;
	outImage->height = original->height;
	outImage->pic = (float **) malloc(outImage->height*sizeof(float *));
	if(outImage->pic == NULL)
		return NULL;
	
    getCannyEdgesClassic_loop_3(&i, &outImage);


	/*generate masks*/
	normalizeFactor = 1.0/(2.0*M_PI*sigma*sigma);
	
    getCannyEdgesClassic_loop_4(&p, &mr, &q, &maskval, &normalizeFactor, &sigma, &maskX, &centY, &centX, &maskY);


	/*convolve masks with picture*/
	
    getCannyEdgesClassic_loop_5(&i, &mr, &original, &j, &sum1, &sum2, &p, &q, &maskX, &centY, &centX, &maskY, &outpicX, &outpicY);


	/*compute strength/magnitude*/
	
    getCannyEdgesClassic_loop_6(&i, &original, &j, &outImage, &mr, &outpicX, &outpicY);

		
	if(percent < 0 || percent > 1)
		percent = 0.20;
		
	/* scale to (0,1) for canny -- necessary? */
	max = 0.0;
	
    getCannyEdgesClassic_loop_7(&i, &outImage, &j, &max);

	
    getCannyEdgesClassic_loop_8(&i, &outImage, &j, &max);

	
	/* allocate peaks image */
	peaks = (Image *) malloc(sizeof(Image));
	peaks->width = original->width;
	peaks->height = original->height;
	peaks->pic = (float **) malloc(peaks->height*sizeof(float *));
	
    getCannyEdgesClassic_loop_9(&i, &peaks);


	/*edge detection/peaks of ridges*/
	
    getCannyEdgesClassic_loop_10(&i, &peaks, &j, &xDiff, &outpicX, &yDiff, &outpicY, &angle, &outImage);


	/*compute high/low thresholds*/
	
    getCannyEdgesClassic_loop_11(&i, &outImage, &j, hist);

	sum = 0;
	picPercent = (int) (percent*outImage->width*outImage->height);
	
    getCannyEdgesClassic_loop_12(&i, &sum, hist, &picPercent, &HI);

	LO = 0.35*HI;

	/* allocate canny image */
	canny = (Image *) malloc(sizeof(Image));
	canny->width = original->width;
	canny->height = original->height;
	canny->pic = (float **) malloc(canny->height*sizeof(float *));
	
    getCannyEdgesClassic_loop_13(&i, &canny);

	
    getCannyEdgesClassic_loop_14(&i, &canny, &j);


	/* hysterisis/double thresholding */
	
    getCannyEdgesClassic_loop_15(&i, &outImage, &j, &peaks, &HI, &canny, &LO);


	
    getCannyEdgesClassic_loop_16(&i, &peaks, &j, &canny);

	
	/* clear border pixels */
	
    getCannyEdgesClassic_loop_17(&i, &mr, &j, &canny);

	
    getCannyEdgesClassic_loop_18(&i, &canny, &mr, &j);

	
    getCannyEdgesClassic_loop_19(&i, &canny, &j, &mr);

	
    getCannyEdgesClassic_loop_20(&i, &canny, &j, &mr);


	/* deallocate memory */
	
    getCannyEdgesClassic_loop_21(&i, &peaks, &outImage);

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
		
    getCannyEdges_loop_22(&i, &original, &j, &xDeriv, &yDeriv);

	}
	
	if(percent < 0 || percent > 1)
		percent = 0.20;
		
	/* scale to (0,1) for canny -- necessary? */
	max = 0.0;
	
    getCannyEdges_loop_23(&i, &original, &j, &max);

	
    getCannyEdges_loop_24(&i, &original, &j, &max);

	
	/* allocate peaks image */
	peaks = (Image *) malloc(sizeof(Image));
	peaks->width = original->width;
	peaks->height = original->height;
	peaks->pic = (float **) malloc(peaks->height*sizeof(float *));
	
    getCannyEdges_loop_25(&i, &peaks);


	/*edge detection/peaks of ridges*/
	
    getCannyEdges_loop_26(&i, &peaks, &j, &angleSource, &xDiff, &xDeriv, &yDiff, &yDeriv, &angle, &original);


	/*compute high/low thresholds*/
	
    getCannyEdges_loop_27(&i, &original, &j, hist);

	sum = 0;
	picPercent = (int) (percent*original->width*original->height);
	
    getCannyEdges_loop_28(&i, &sum, hist, &picPercent, &HI);

	LO = 0.35*HI;

	/* allocate canny image */
	canny = (Image *) malloc(sizeof(Image));
	canny->width = original->width;
	canny->height = original->height;
	canny->pic = (float **) malloc(canny->height*sizeof(float *));
	
    getCannyEdges_loop_29(&i, &canny);

	
    getCannyEdges_loop_30(&i, &canny, &j);


	/* hysterisis/double thresholding */
	
    getCannyEdges_loop_31(&i, &original, &j, &peaks, &HI, &canny, &LO);


	
    getCannyEdges_loop_32(&i, &peaks, &j, &canny);

	
	/* deallocate memory */
	
    getCannyEdges_loop_33(&i, &peaks, &original);

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
	
    newCannyPPM_loop_34(&i, &gSize, &gaussianx);

        gaussiany = (double **) malloc(gSize*sizeof(double *));
	
    newCannyPPM_loop_35(&i, &gSize, &gaussiany);

        
	/* constant */
	kappa = 1 / sqrt(2*M_PI*sigma*sigma);
        
	/* calculate mask */
	//q and p switched
	
    newCannyPPM_loop_36(&i, &gSize, &q, &j, &p, &gaussianx, &kappa, &sigma, &gaussiany);

	
	/* allocate magnitude and theta arrays */
	new->magnitude = (double **) malloc(new->height*sizeof(double *));
	
    newCannyPPM_loop_37(&i, &new);

	new->theta = (double **) malloc(new->height*sizeof(double *));
	
    newCannyPPM_loop_38(&i, &new);

	
	/* convolve */
	
    newCannyPPM_loop_39(&y, &new, &x, &dgxSum, &dgySum, &xrSum, &yrSum, &xgSum, &ygSum, &xbSum, &ybSum, &i, &gSize, &q, &j, &p, &image, &gaussianx, &gaussiany, &rMag, &gMag, &bMag);

	
	max = 0.0;
	
    newCannyPPM_loop_40(&i, &new, &j, &max);

	
    newCannyPPM_loop_41(&i, &new, &j, &max);

	
	/* deallocate gaussianx, gaussiany */
	
    newCannyPPM_loop_42(&i, &gSize, &gaussianx, &gaussiany);

        free(gaussianx);
	free(gaussiany);
	
	/* nonmaxima suppresion */
	/* allocate peaks array */
	peaks = (int **) malloc(new->height*sizeof(int *));
	
    newCannyPPM_loop_43(&i, &new, &peaks);

	// iterate through the gradient image
	
    newCannyPPM_loop_44(&y, &new, &x, &peaks, &sector);

	
	debug = allocateImageFromDoubles(new->magnitude, new->width, new->height);
	writePGMFile(debug, "magnitudes.pgm");
	
	debug = allocateImageFromInts(peaks, 255, new->width, new->height);
	writePGMFile(debug, "peaks.pgm");
		
	findThresholds(new, &low, &high);
	
	/* eliminate low values */
	
    newCannyPPM_loop_45(&y, &new, &x, &peaks, &low);

	
	new->edges = allocateImage(new->width, new->height);
        
	/* keep high values */
	
    newCannyPPM_loop_46(&y, &new, &x, &peaks, &high);

	
	writePGMFile(new->edges, "canny_thresh.pgm");
	
	/* allocate and initialize traced array */
	traced = (int **) malloc(new->height*sizeof(int *));
	
    newCannyPPM_loop_47(&i, &new, &traced);

	
    newCannyPPM_loop_48(&i, &new, &j, &traced);

        
        // iterate through the pixels, if we find an edge that hasn't been traced yet, start
        //   the recursion there
	
    newCannyPPM_loop_49(&y, &new, &x, &traced, &low, &peaks);

	
	/* clear border pixels, assumes picture is at least border x border */
	border = (int) (3*new->sigma);
	
    newCannyPPM_loop_50(&j, &border, &i, &new);

	
    newCannyPPM_loop_51(&j, &new, &border, &i);

	
    newCannyPPM_loop_52(&i, &border, &j, &new);

	
    newCannyPPM_loop_53(&i, &new, &border, &j);

	
	/* deallocate peaks and traced arrays */
	
    newCannyPPM_loop_54(&i, &new, &peaks, &traced);

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
	
    newCannyPGMTwoSources_loop_55(&i, &new);

	new->theta = (double **) malloc(new->height*sizeof(double *));
	
    newCannyPGMTwoSources_loop_56(&i, &new);

	
	/* initialize magnitudes and thetas */
	
    newCannyPGMTwoSources_loop_57(&i, &new, &j, &gradient, &gaussian);

	
	/* nonmaxima suppresion */
	/* allocate peaks array */
	peaks = (int **) malloc(new->height*sizeof(int *));
	
    newCannyPGMTwoSources_loop_58(&i, &new, &peaks);

	
	/* find peaks */
	
    newCannyPGMTwoSources_loop_59(&y, &new, &x, &peaks, &sector);

	
	findThresholds(new, &low, &high);
	
	/* eliminate low values */
	
    newCannyPGMTwoSources_loop_60(&y, &new, &x, &peaks, &low);

	
	new->edges = allocateImage(new->width, new->height);
        
	/* keep high values */
	
    newCannyPGMTwoSources_loop_61(&y, &new, &x, &peaks, &high);

	
	/* allocate and initialize traced array */
	traced = (int **) malloc(new->height*sizeof(int *));
	
    newCannyPGMTwoSources_loop_62(&i, &new, &traced);

	
    newCannyPGMTwoSources_loop_63(&i, &new, &j, &traced);

        
        // iterate through the pixels, if we find an edge that hasn't been traced yet, start
        //   the recursion there
	
    newCannyPGMTwoSources_loop_64(&y, &new, &x, &traced, &low, &peaks);

	
	/* deallocate peaks and traced arrays */
	
    newCannyPGMTwoSources_loop_65(&i, &new, &peaks, &traced);

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
	
    findThresholds_loop_66(&i, &this, &j, hist);

        
        // work your way down the  histogram from the right, adding up the area
        //   if the area you're at is greater than the percent of pixels you want to choose,
        //   finish
	
    findThresholds_loop_67(&c, &area, hist, &percent, &this);

        
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
	
    freeCanny_loop_68(&i, &this);

	free(this);
}
