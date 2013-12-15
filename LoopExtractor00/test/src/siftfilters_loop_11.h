#ifndef siftfilters_loop_11_h_
#define siftfilters_loop_11_h_

#include "siftfilters.h"

void isFingerTip_loop_8(int *i, int *foundEdges, double distance[], int edgeX[], KeyPoint * *point, int edgeY[]);
void getAdjustment_loop_3(int *i, Matrix * *matrixCopy);
void isFingerTip_loop_4(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle);
void isFingerTip_loop_9(int *i, int *foundEdges, int *j, double distance[], int *re_arg_pa1_9, int *re_arg_pa2_9);
void isFingerTip_loop_7(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle, Image * *diffImage);
void isFingerTip_loop_10(int *i, int *foundEdges, double distance[]);
void localizeIsWeak_loop_1(int *i, int *rows, Matrix * *adj);
void isFingerTip_loop_6(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle);
void isFingerTip_loop_11(int *i, int *foundEdges);
void getAdjustment_loop_2(int *i, gsl_vector * *result, double *sum, gsl_vector * *d);
void isFingerTip_loop_5(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle);

#endif
