#ifndef image_loop_26_h_
#define image_loop_26_h_

#include "image.h"

void exportCSV_loop_23(int *i, Image * *image, int *j, FILE * *output);
void allocateImage_loop_1(int *i, Image * *new, int *j, int *re_arg_pa1_1, Image * *re_arg_pa2_1);
void doubleCanny_loop_10(int *i, Image * *original, int *j, Image * *doubled);
void getDerivative_loop_18(int *i, Image * *original, float ** *imageX, float ** *imageY);
void getDerivative_loop_17(int *i, Image * *original, int *j, Image * *newImage, float ** *imageX, float ** *imageY);
void halveImage_loop_6(int *i, Image * *halved, int *j, Image * *original);
void getDerivativeY_loop_16(int *j, Image * *original, int *i, double *sum2, int *p, int maskY[], float ** *imageY);
void allocateImageFromInts_loop_4(int *i, Image * *new, int *j, int ** *array, int *maxValue);
void drawLine_loop_21(int *i, int *x1, int *x2, int *comp, int *y1, int *dy, int *dx, Image * *image);
void isPeak_loop_20(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakL, int *re_arg_pa1_20, int *re_arg_pa2_20);
void getDerivativeX_loop_13(int *i, Image * *original, float ** *imageX);
void isPeak_loop_19(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakR, int *re_arg_pa1_19, int *re_arg_pa2_19);
void allocateImageFromDoubles_loop_3(int *i, Image * *new, int *j, double ** *array);
void subtractImages_loop_11(int *i, Image * *one, int *j, Image * *newImage, Image * *two);
void cloneImage_loop_12(int *i, Image * *newImage, int *j, Image * *original);
void freeImage_loop_5(int *i, Image * *image);
void getDerivativeY_loop_15(int *i, Image * *original, float ** *imageY);
void findMax_loop_25(int *i, Image * *image, int *j, float *max);
void doubleImage_loop_7(int *i, Image * *original, int *j, Image * *doubled);
void doubleCanny_loop_9(int *i, Image * *original, int *j, Image * *doubled);
void findMin_loop_24(int *i, Image * *image, int *j, float *min);
void getDerivativeX_loop_14(int *i, Image * *original, int *j, double *sum1, int *p, int maskX[], float ** *imageX);
void doubleCanny_loop_8(int *i, Image * *doubled);
void drawLine_loop_22(int *i, int *y1, int *y2, int *comp, int *x1, int *dx, int *dy, Image * *image);
void allocateImage_loop_2(int *i, Image * *new, int *j);
void convertToRGB_loop_26(int *i, Image * *image, int *j, ImageRGB * *new);

#endif
