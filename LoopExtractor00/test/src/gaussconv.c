#include "gaussconv_loop_23.h"
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
	
    gaussianD1Convolution2D_loop_1(&i, &maskSize, &maskX, &maskY);


	outpicX = (double **) malloc(original->height*sizeof(double *));
	outpicY = (double **) malloc(original->height*sizeof(double *));
	if(outpicX == NULL || outpicY == NULL)
		return NULL;
	
    gaussianD1Convolution2D_loop_2(&i, &original, &outpicX, &outpicY);


	outImage = (Image *) malloc(sizeof(Image));
	outImage->width = original->width;
	outImage->height = original->height;
	outImage->pic = (float **) malloc(outImage->height*sizeof(float *));
	if(outImage->pic == NULL)
		return NULL;
	
    gaussianD1Convolution2D_loop_3(&i, &outImage);


	/*generate masks*/
	normalizeFactor = 1.0/(2.0*M_PI*sigma*sigma);
	
    gaussianD1Convolution2D_loop_4(&p, &mr, &q, &maskval, &normalizeFactor, &sigma, &maskX, &centY, &centX, &maskY);


	/*convolve masks with picture*/
        
    gaussianD1Convolution2D_loop_5(&i, &mr, &original, &j, &sum1, &sum2, &p, &q, &maskX, &centY, &centX, &maskY, &outpicX, &outpicY);


	/*compute strength/magnitude*/
        
    gaussianD1Convolution2D_loop_6(&i, &original, &j, &outImage, &mr, &outpicX, &outpicY);


	/* deallocate memory */
	
    gaussianD1Convolution2D_loop_7(&i, &maskSize, &maskX, &maskY);

	free(maskX);
	free(maskY);

	
    gaussianD1Convolution2D_loop_8(&i, &original, &outpicX, &outpicY);

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
	
    gaussianConvolution2D_loop_9(&i, &maskSize, &mask);


	outpic = (double **) malloc(original->height*sizeof(double *));
	if(outpic == NULL)
		return NULL;
	
    gaussianConvolution2D_loop_10(&i, &original, &outpic);


	outImage = (Image *) malloc(sizeof(Image));
	outImage->width = original->width;
	outImage->height = original->height;
	outImage->pic = (float **) malloc(outImage->height*sizeof(float *));
	if(outImage->pic == NULL)
		return NULL;
	
    gaussianConvolution2D_loop_11(&i, &outImage);


	/*generate masks*/
	normalizeFactor = 1.0/(2.0*M_PI*sigma*sigma);
	
    gaussianConvolution2D_loop_12(&p, &mr, &q, &maskval, &normalizeFactor, &sigma, &mask, &centY, &centX);


	/*convolve masks with picture*/
        
    gaussianConvolution2D_loop_13(&i, &mr, &original, &j, &sum, &p, &q, &mask, &centY, &centX, &outpic);


	/*compute strength/magnitude*/
        
    gaussianConvolution2D_loop_14(&i, &original, &j, &outImage, &mr, &outpic);


	/* deallocate memory */
	
    gaussianConvolution2D_loop_15(&i, &maskSize, &mask);

	free(mask);

	
    gaussianConvolution2D_loop_16(&i, &original, &outpic);

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
	
    gaussianConvolution2DFast_loop_17(&i, &original, &outpicY);


	outImage = (Image *) malloc(sizeof(Image));
	outImage->width = original->width;
	outImage->height = original->height;
	outImage->pic = (float **) malloc(outImage->height*sizeof(float *));
	if(outImage->pic == NULL)
		return NULL;
	
    gaussianConvolution2DFast_loop_18(&i, &outImage);


	/* initialize */
	
    gaussianConvolution2DFast_loop_19(&i, &original, &j, &outpicY, &outImage);


	/*generate mask*/
	normalizeFactor = 1.0/(sqrt(2.0*M_PI)*sigma);
	
    gaussianConvolution2DFast_loop_20(&i, &maskSize, &p, &maskMiddle, &mask, &normalizeFactor, &sigma);


	/*convolve 1D -- vertical pass*/
	
    gaussianConvolution2DFast_loop_21(&i, &original, &j, &sum, &isOut, &outBound, &p, &maskSize, &q, &maskMiddle, &mask, &outpicY);


	/*convolve 1D -- horizontal pass*/
	
    gaussianConvolution2DFast_loop_22(&i, &original, &j, &sum, &isOut, &outBound, &p, &maskSize, &q, &maskMiddle, &mask, &outpicY, &outImage);


	/* deallocate memory */
	free(mask);
	
    gaussianConvolution2DFast_loop_23(&i, &original, &outpicY);

	free(outpicY);

	return outImage;
}
