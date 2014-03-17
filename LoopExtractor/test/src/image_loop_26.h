#ifndef image_loop_26_h_
#define image_loop_26_h_

/*
*  Jan Prokaj
*  May 11, 2005
*  based on ImageMap.cs -- Sebastian Nowozin (nowozin@cs.tu-berlin.de)
*/
#include "image.h"


int convertToRGB_loop_26(int *i, Image * *image, int *j, ImageRGB * *new);
int getDerivativeX_loop_13(int *i, Image * *original, float ** *imageX);
int drawLine_loop_21(int *i, int *x1, int *x2, int *comp, int *y1, int *dy, int *dx, Image * *image);
int getDerivative_loop_17(int *i, Image * *original, int *j, Image * *newImage, float ** *imageX, float ** *imageY);
int getDerivativeY_loop_16(int *j, Image * *original, int *i, double *sum2, int *p, int maskY[], float ** *imageY);
int allocateImageFromInts_loop_4(int *i, Image * *new, int *j, int ** *array, int *maxValue);
int doubleCanny_loop_10(int *i, Image * *original, int *j, Image * *doubled);
int cloneImage_loop_12(int *i, Image * *newImage, int *j, Image * *original);
int freeImage_loop_5(int *i, Image * *image);
int getDerivative_loop_18(int *i, Image * *original, float ** *imageX, float ** *imageY);
int findMin_loop_24(int *i, Image * *image, int *j, float *min);
int findMax_loop_25(int *i, Image * *image, int *j, float *max);
int doubleCanny_loop_8(int *i, Image * *doubled);
int allocateImage_loop_2(int *i, Image * *new, int *j);
int getDerivativeX_loop_14(int *i, Image * *original, int *j, double *sum1, int *p, int maskX[], float ** *imageX);
int drawLine_loop_22(int *i, int *y1, int *y2, int *comp, int *x1, int *dx, int *dy, Image * *image);
int doubleCanny_loop_9(int *i, Image * *original, int *j, Image * *doubled);
int allocateImageFromDoubles_loop_3(int *i, Image * *new, int *j, double ** *array);
int doubleImage_loop_7(int *i, Image * *original, int *j, Image * *doubled);
int isPeak_loop_19(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakR, int *re_arg_pa1_19, int *re_arg_pa2_19);
int subtractImages_loop_11(int *i, Image * *one, int *j, Image * *newImage, Image * *two);
int halveImage_loop_6(int *i, Image * *halved, int *j, Image * *original);
int getDerivativeY_loop_15(int *i, Image * *original, float ** *imageY);
int allocateImage_loop_1(int *i, Image * *new, int *j, int *re_arg_pa1_1, Image * *re_arg_pa2_1);
int isPeak_loop_20(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakL, int *re_arg_pa1_20, int *re_arg_pa2_20);
int exportCSV_loop_23(int *i, Image * *image, int *j, FILE * *output);

#endif
