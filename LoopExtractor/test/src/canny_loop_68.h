#ifndef canny_loop_68_h_
#define canny_loop_68_h_

/*
*  Jan Prokaj
*  May 11, 2005
*/
#include "canny.h"

//basic algorithm, no performance improvements

int newCannyPGMTwoSources_loop_64(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks);
int getCannyEdges_loop_31(int *i, Image * *original, int *j, Image * *peaks, float *HI, Image * *canny, float *LO);
int getCannyEdges_loop_25(int *i, Image * *peaks);
int getCannyEdges_loop_32(int *i, Image * *peaks, int *j, Image * *canny);
int newCannyPPM_loop_38(int *i, Canny * *new);
int newCannyPPM_loop_48(int *i, Canny * *new, int *j, int ** *traced);
int getCannyEdgesClassic_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
int newCannyPPM_loop_34(int *i, int *gSize, double ** *gaussianx);
int newCannyPGMTwoSources_loop_62(int *i, Canny * *new, int ** *traced);
int getCannyEdgesClassic_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY);
int getCannyEdges_loop_30(int *i, Image * *canny, int *j);
int getCannyEdgesClassic_loop_7(int *i, Image * *outImage, int *j, float *max);
int getCannyEdgesClassic_loop_19(int *i, Image * *canny, int *j, int *mr);
int getCannyEdgesClassic_loop_13(int *i, Image * *canny);
int getCannyEdges_loop_23(int *i, Image * *original, int *j, float *max);
int newCannyPPM_loop_44(int *y, Canny * *new, int *x, int ** *peaks, int *sector);
int newCannyPPM_loop_51(int *j, Canny * *new, int *border, int *i);
int newCannyPGMTwoSources_loop_59(int *y, Canny * *new, int *x, int ** *peaks, int *sector);
int newCannyPGMTwoSources_loop_63(int *i, Canny * *new, int *j, int ** *traced);
int newCannyPPM_loop_49(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks);
int getCannyEdgesClassic_loop_18(int *i, Image * *canny, int *mr, int *j);
int getCannyEdges_loop_24(int *i, Image * *original, int *j, float *max);
int getCannyEdgesClassic_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, float *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY);
int findThresholds_loop_67(int *c, int *area, int hist[], double *percent, Canny * *this);
int newCannyPPM_loop_40(int *i, Canny * *new, int *j, double *max);
int getCannyEdgesClassic_loop_9(int *i, Image * *peaks);
int newCannyPPM_loop_39(int *y, Canny * *new, int *x, double *dgxSum, double *dgySum, double *xrSum, double *yrSum, double *xgSum, double *ygSum, double *xbSum, double *ybSum, int *i, int *gSize, int *q, int *j, int *p, ImageRGB * *image, double ** *gaussianx, double ** *gaussiany, double *rMag, double *gMag, double *bMag);
int getCannyEdges_loop_28(int *i, int *sum, int hist[], float *picPercent, float *HI);
int newCannyPPM_loop_50(int *j, int *border, int *i, Canny * *new);
int newCannyPGMTwoSources_loop_56(int *i, Canny * *new);
int getCannyEdges_loop_27(int *i, Image * *original, int *j, int hist[]);
int newCannyPPM_loop_41(int *i, Canny * *new, int *j, double *max);
int getCannyEdgesClassic_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY);
int newCannyPGMTwoSources_loop_65(int *i, Canny * *new, int ** *peaks, int ** *traced);
int getCannyEdges_loop_26(int *i, Image * *peaks, int *j, Image * *angleSource, float *xDiff, float ** *xDeriv, float *yDiff, float ** *yDeriv, double *angle, Image * *original);
int newCannyPGMTwoSources_loop_55(int *i, Canny * *new);
int getCannyEdgesClassic_loop_14(int *i, Image * *canny, int *j);
int newCannyPPM_loop_54(int *i, Canny * *new, int ** *peaks, int ** *traced);
int newCannyPGMTwoSources_loop_60(int *y, Canny * *new, int *x, int ** *peaks, double *low);
int getCannyEdgesClassic_loop_11(int *i, Image * *outImage, int *j, int hist[]);
int newCannyPGMTwoSources_loop_57(int *i, Canny * *new, int *j, Image * *gradient, Image * *gaussian);
int newCannyPPM_loop_45(int *y, Canny * *new, int *x, int ** *peaks, double *low);
int findThresholds_loop_66(int *i, Canny * *this, int *j, int hist[]);
int getCannyEdgesClassic_loop_17(int *i, int *mr, int *j, Image * *canny);
int getCannyEdgesClassic_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY);
int getCannyEdgesClassic_loop_10(int *i, Image * *peaks, int *j, float *xDiff, double ** *outpicX, float *yDiff, double ** *outpicY, double *angle, Image * *outImage);
int getCannyEdgesClassic_loop_21(int *i, Image * *peaks, Image * *outImage);
int getCannyEdges_loop_22(int *i, Image * *original, int *j, float ** *xDeriv, float ** *yDeriv);
int newCannyPPM_loop_46(int *y, Canny * *new, int *x, int ** *peaks, double *high);
int getCannyEdges_loop_33(int *i, Image * *peaks, Image * *original);
int newCannyPPM_loop_47(int *i, Canny * *new, int ** *traced);
int getCannyEdgesClassic_loop_12(int *i, int *sum, int hist[], float *picPercent, float *HI);
int getCannyEdgesClassic_loop_15(int *i, Image * *outImage, int *j, Image * *peaks, float *HI, Image * *canny, float *LO);
int newCannyPGMTwoSources_loop_58(int *i, Canny * *new, int ** *peaks);
int newCannyPPM_loop_36(int *i, int *gSize, int *q, int *j, int *p, double ** *gaussianx, double *kappa, double *sigma, double ** *gaussiany);
int newCannyPPM_loop_35(int *i, int *gSize, double ** *gaussiany);
int getCannyEdgesClassic_loop_20(int *i, Image * *canny, int *j, int *mr);
int newCannyPGMTwoSources_loop_61(int *y, Canny * *new, int *x, int ** *peaks, double *high);
int getCannyEdges_loop_29(int *i, Image * *canny);
int getCannyEdgesClassic_loop_16(int *i, Image * *peaks, int *j, Image * *canny);
int freeCanny_loop_68(int *i, Canny * *this);
int newCannyPPM_loop_37(int *i, Canny * *new);
int getCannyEdgesClassic_loop_3(int *i, Image * *outImage);
int newCannyPPM_loop_53(int *i, Canny * *new, int *border, int *j);
int newCannyPPM_loop_42(int *i, int *gSize, double ** *gaussianx, double ** *gaussiany);
int getCannyEdgesClassic_loop_8(int *i, Image * *outImage, int *j, float *max);
int newCannyPPM_loop_43(int *i, Canny * *new, int ** *peaks);
int newCannyPPM_loop_52(int *i, int *border, int *j, Canny * *new);

#endif
