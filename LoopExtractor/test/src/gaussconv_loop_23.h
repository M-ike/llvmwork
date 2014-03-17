#ifndef gaussconv_loop_23_h_
#define gaussconv_loop_23_h_

/*
*  Jan Prokaj
*  January 31, 2005
*/

#include "gaussconv.h"

//Does a gaussian 1st derivative (G') convolution on the passsed in image.
//The resulting image is dynamically allocated and returned
//via a pointer

int gaussianD1Convolution2D_loop_7(int *i, int *maskSize, double ** *maskX, double ** *maskY);
int gaussianConvolution2DFast_loop_17(int *i, Image * *original, double ** *outpicY);
int gaussianConvolution2D_loop_11(int *i, Image * *outImage);
int gaussianConvolution2DFast_loop_20(int *i, int *maskSize, int *p, int *maskMiddle, double * *mask, double *normalizeFactor, double *sigma);
int gaussianConvolution2D_loop_9(int *i, int *maskSize, double ** *mask);
int gaussianConvolution2D_loop_14(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpic);
int gaussianD1Convolution2D_loop_8(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
int gaussianConvolution2DFast_loop_22(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY, Image * *outImage);
int gaussianD1Convolution2D_loop_3(int *i, Image * *outImage);
int gaussianConvolution2DFast_loop_18(int *i, Image * *outImage);
int gaussianD1Convolution2D_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY);
int gaussianConvolution2DFast_loop_23(int *i, Image * *original, double ** *outpicY);
int gaussianConvolution2DFast_loop_19(int *i, Image * *original, int *j, double ** *outpicY, Image * *outImage);
int gaussianConvolution2D_loop_15(int *i, int *maskSize, double ** *mask);
int gaussianConvolution2D_loop_16(int *i, Image * *original, double ** *outpic);
int gaussianConvolution2DFast_loop_21(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY);
int gaussianConvolution2D_loop_12(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *mask, int *centY, int *centX);
int gaussianD1Convolution2D_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
int gaussianD1Convolution2D_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY);
int gaussianD1Convolution2D_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY);
int gaussianConvolution2D_loop_10(int *i, Image * *original, double ** *outpic);
int gaussianConvolution2D_loop_13(int *i, int *mr, Image * *original, int *j, double *sum, int *p, int *q, double ** *mask, int *centY, int *centX, double ** *outpic);
int gaussianD1Convolution2D_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY);

#endif
