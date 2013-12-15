#ifndef canny_loop_68_h_
#define canny_loop_68_h_

#include "canny.h"

void getCannyEdges_loop_28(int *i, int *sum, int hist[], float *picPercent, float *HI);
void getCannyEdges_loop_30(int *i, Image * *canny, int *j);
void newCannyPPM_loop_50(int *j, int *border, int *i, Canny * *new);
void getCannyEdges_loop_27(int *i, Image * *original, int *j, int hist[]);
void getCannyEdgesClassic_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY);
void getCannyEdgesClassic_loop_19(int *i, Image * *canny, int *j, int *mr);
void getCannyEdgesClassic_loop_8(int *i, Image * *outImage, int *j, float *max);
void newCannyPGMTwoSources_loop_56(int *i, Canny * *new);
void getCannyEdgesClassic_loop_13(int *i, Image * *canny);
void getCannyEdges_loop_23(int *i, Image * *original, int *j, float *max);
void getCannyEdges_loop_26(int *i, Image * *peaks, int *j, Image * *angleSource, float *xDiff, float ** *xDeriv, float *yDiff, float ** *yDeriv, double *angle, Image * *original);
void newCannyPGMTwoSources_loop_55(int *i, Canny * *new);
void newCannyPPM_loop_52(int *i, int *border, int *j, Canny * *new);
void getCannyEdgesClassic_loop_20(int *i, Image * *canny, int *j, int *mr);
void getCannyEdges_loop_24(int *i, Image * *original, int *j, float *max);
void newCannyPPM_loop_34(int *i, int *gSize, double ** *gaussianx);
void newCannyPGMTwoSources_loop_63(int *i, Canny * *new, int *j, int ** *traced);
void getCannyEdges_loop_33(int *i, Image * *peaks, Image * *original);
void getCannyEdgesClassic_loop_3(int *i, Image * *outImage);
void newCannyPPM_loop_53(int *i, Canny * *new, int *border, int *j);
void newCannyPPM_loop_37(int *i, Canny * *new);
void newCannyPGMTwoSources_loop_62(int *i, Canny * *new, int ** *traced);
void newCannyPPM_loop_43(int *i, Canny * *new, int ** *peaks);
void newCannyPPM_loop_42(int *i, int *gSize, double ** *gaussianx, double ** *gaussiany);
void newCannyPPM_loop_44(int *y, Canny * *new, int *x, int ** *peaks, int *sector);
void getCannyEdgesClassic_loop_15(int *i, Image * *outImage, int *j, Image * *peaks, float *HI, Image * *canny, float *LO);
void newCannyPGMTwoSources_loop_59(int *y, Canny * *new, int *x, int ** *peaks, int *sector);
void getCannyEdges_loop_31(int *i, Image * *original, int *j, Image * *peaks, float *HI, Image * *canny, float *LO);
void getCannyEdges_loop_25(int *i, Image * *peaks);
void getCannyEdges_loop_32(int *i, Image * *peaks, int *j, Image * *canny);
void newCannyPPM_loop_38(int *i, Canny * *new);
void newCannyPPM_loop_35(int *i, int *gSize, double ** *gaussiany);
void newCannyPPM_loop_48(int *i, Canny * *new, int *j, int ** *traced);
void getCannyEdges_loop_29(int *i, Image * *canny);
void newCannyPGMTwoSources_loop_64(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks);
void getCannyEdgesClassic_loop_16(int *i, Image * *peaks, int *j, Image * *canny);
void newCannyPPM_loop_46(int *y, Canny * *new, int *x, int ** *peaks, double *high);
void getCannyEdgesClassic_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY);
void newCannyPPM_loop_47(int *i, Canny * *new, int ** *traced);
void getCannyEdgesClassic_loop_12(int *i, int *sum, int hist[], float *picPercent, float *HI);
void getCannyEdgesClassic_loop_21(int *i, Image * *peaks, Image * *outImage);
void newCannyPGMTwoSources_loop_58(int *i, Canny * *new, int ** *peaks);
void getCannyEdgesClassic_loop_11(int *i, Image * *outImage, int *j, int hist[]);
void newCannyPPM_loop_36(int *i, int *gSize, int *q, int *j, int *p, double ** *gaussianx, double *kappa, double *sigma, double ** *gaussiany);
void newCannyPGMTwoSources_loop_60(int *y, Canny * *new, int *x, int ** *peaks, double *low);
void newCannyPGMTwoSources_loop_57(int *i, Canny * *new, int *j, Image * *gradient, Image * *gaussian);
void newCannyPGMTwoSources_loop_61(int *y, Canny * *new, int *x, int ** *peaks, double *high);
void getCannyEdgesClassic_loop_17(int *i, int *mr, int *j, Image * *canny);
void freeCanny_loop_68(int *i, Canny * *this);
void findThresholds_loop_66(int *i, Canny * *this, int *j, int hist[]);
void getCannyEdges_loop_22(int *i, Image * *original, int *j, float ** *xDeriv, float ** *yDeriv);
void getCannyEdgesClassic_loop_7(int *i, Image * *outImage, int *j, float *max);
void getCannyEdgesClassic_loop_10(int *i, Image * *peaks, int *j, float *xDiff, double ** *outpicX, float *yDiff, double ** *outpicY, double *angle, Image * *outImage);
void newCannyPPM_loop_51(int *j, Canny * *new, int *border, int *i);
void newCannyPGMTwoSources_loop_65(int *i, Canny * *new, int ** *peaks, int ** *traced);
void getCannyEdgesClassic_loop_14(int *i, Image * *canny, int *j);
void newCannyPPM_loop_54(int *i, Canny * *new, int ** *peaks, int ** *traced);
void getCannyEdgesClassic_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, float *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY);
void getCannyEdgesClassic_loop_18(int *i, Image * *canny, int *mr, int *j);
void findThresholds_loop_67(int *c, int *area, int hist[], double *percent, Canny * *this);
void newCannyPPM_loop_39(int *y, Canny * *new, int *x, double *dgxSum, double *dgySum, double *xrSum, double *yrSum, double *xgSum, double *ygSum, double *xbSum, double *ybSum, int *i, int *gSize, int *q, int *j, int *p, ImageRGB * *image, double ** *gaussianx, double ** *gaussiany, double *rMag, double *gMag, double *bMag);
void newCannyPPM_loop_40(int *i, Canny * *new, int *j, double *max);
void newCannyPPM_loop_41(int *i, Canny * *new, int *j, double *max);
void newCannyPPM_loop_45(int *y, Canny * *new, int *x, int ** *peaks, double *low);
void newCannyPPM_loop_49(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks);
void getCannyEdgesClassic_loop_9(int *i, Image * *peaks);
void getCannyEdgesClassic_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY);
void getCannyEdgesClassic_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY);

#endif
