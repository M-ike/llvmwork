#ifndef gaussconv_loop_23_h_
#define gaussconv_loop_23_h_

#include "gaussconv.h"

void gaussianConvolution2DFast_loop_17(int *i, Image * *original, double ** *outpicY);
void gaussianD1Convolution2D_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY);
void gaussianD1Convolution2D_loop_7(int *i, int *maskSize, double ** *maskX, double ** *maskY);
void gaussianD1Convolution2D_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY);
void gaussianConvolution2D_loop_10(int *i, Image * *original, double ** *outpic);
void gaussianConvolution2DFast_loop_19(int *i, Image * *original, int *j, double ** *outpicY, Image * *outImage);
void gaussianConvolution2D_loop_14(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpic);
void gaussianConvolution2DFast_loop_23(int *i, Image * *original, double ** *outpicY);
void gaussianConvolution2DFast_loop_22(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY, Image * *outImage);
void gaussianD1Convolution2D_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY);
void gaussianConvolution2DFast_loop_18(int *i, Image * *outImage);
void gaussianConvolution2DFast_loop_21(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY);
void gaussianD1Convolution2D_loop_8(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
void gaussianConvolution2DFast_loop_20(int *i, int *maskSize, int *p, int *maskMiddle, double * *mask, double *normalizeFactor, double *sigma);
void gaussianConvolution2D_loop_9(int *i, int *maskSize, double ** *mask);
void gaussianConvolution2D_loop_12(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *mask, int *centY, int *centX);
void gaussianConvolution2D_loop_15(int *i, int *maskSize, double ** *mask);
void gaussianConvolution2D_loop_16(int *i, Image * *original, double ** *outpic);
void gaussianD1Convolution2D_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY);
void gaussianConvolution2D_loop_13(int *i, int *mr, Image * *original, int *j, double *sum, int *p, int *q, double ** *mask, int *centY, int *centX, double ** *outpic);
void gaussianD1Convolution2D_loop_3(int *i, Image * *outImage);
void gaussianD1Convolution2D_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
void gaussianConvolution2D_loop_11(int *i, Image * *outImage);

#endif
