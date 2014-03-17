#ifndef scalespace_loop_33_h_
#define scalespace_loop_33_h_

/*
*  Jan Prokaj
*  March 5, 2005
*/
#include "scalespace.h"


//Builds the scale space by incrementally convolving until reaching 2*startSigma
//If we want IMAGES_PER_OCTAVE number of usable images, we need to produce + 2
//initial images (or INTERVALS_PER_OCTAVE + 3 as Lowe says). So k is equal
//to 2^(1/s)

int markImage_loop_32(KeyPoint * *current, KeyPoint * *list, double *length, double *degree, int *endX, int *endY, Image * *image, int *arrowX, int *arrowY);
int buildScaleSpaceOctave_loop_1(int *i, ScaleSpace * *space, double *sigma, double *k);
int generateKeypointSingle_loop_19(int *binIndex, double bins[], double *maxGradient, int *maxBin);
int buildScaleSpaceOctave_loop_2(int *i, ScaleSpace * *space, double *sourceBlur, double *imageSigma, double *targetBlur, Image * *original, double *sigma);
int allocateFeatureVector_loop_23(int *i, int *descriptorDim, float *** *matrix);
int capAndNormalizeFV_loop_27(int *i, KeyPoint * *this, int *j, int *k, double *norm);
int capAndNormalizeFV_loop_26(int *i, KeyPoint * *this, int *j, int *k, double *norm);
int buildDifferences_loop_5(int *i, ScaleSpace * *space, int *j);
int buildScaleSpaceEdges_loop_6(int *i, ScaleSpace * *space, ScaleSpace * *original);
int printHistogram_loop_9(int *y, int *yMin, int *yMax, int *x, int *xMin, int *xMax, int *relativeX, KeyPoint * *point, int *relativeY, double *radius, double *yDiff, ScaleSpace * *this, double *xDiff, double *direction, int *binIndex, double *oneBinRad, double *magnitude, double bins[], double *sigma);
int printKeyPoint_loop_31(int *i, KeyPoint * *this, int *j, int *k, FILE * *out, int *count);
int generateKeypointSingle_loop_21(int *binIndex, int isKeypoint[], int *left, int *right, double bins[], double *maxDegreeCorrection, double *maxGradient, double *degree, double *oneBinRad, KeyPoint * *point, int * *extraCount, KeyPoint * *newKey, KeyPoint * *lastAddition, KeyPoint * *list);
int printHistogram_loop_10(int *binIndex, double bins[], double *maxGradient);
int allocateFeatureVector_loop_25(int *i, int *descriptorDim, int *j, int *k, int *directionCount, float *** *matrix);
int buildDifferences_loop_3(int *i, ScaleSpace * *space, ScaleSpace * *original);
int allocate3D_loop_13(int *i, int *height, double *** *matrix, int *width);
int generateKeypointSingle_loop_20(int *binIndex, int *maxBin, int isKeypoint[], double bins[], double *maxGradient, int *left, int *right);
int capAndNormalizeFV_loop_30(int *i, KeyPoint * *this, int *j, int *k, double *norm);
int capAndNormalizeFV_loop_29(int *i, KeyPoint * *this, int *j, int *k, double *norm);
int deallocate3D_loop_15(int *i, int *height, int *j, int *width, double *** *matrix);
int markImageSPoint_loop_33(KeyPoint * *current, KeyPoint * *list, Image * *image, double *scale);
int buildDifferences_loop_4(int *i, ScaleSpace * *space);
int generateKeypoints_loop_16(KeyPoint * *current, KeyPoint * *peaks, ScaleSpace * *this, int *multipleKeypoints);
int buildScaleSpaceCanny_loop_7(int *i, ScaleSpace * *space, ScaleSpace * *derivative, ScaleSpace * *gaussian);
int generateKeypointSingle_loop_18(int *binIndex, double bins[], double *maxGradient);
int getMagnitudesAndOrientations_loop_12(int *i, int *height, int *j, int *width, int *k, int *imageLevels, double *xDiff, ScaleSpace * *this, double *yDiff);
int findPeaks_loop_8(int *i, Image * *current, int *j, float *min, int *isMin, int *p, int *q, Image * *above, Image * *below, KeyPoint * *temp, int *DoGlevel, ScaleSpace * *this, KeyPoint * *peaks, float *max, int *isMax);
int printHistogram_loop_11(int *binIndex, int *barWidth, double bins[], double *maxGradient, int *i);
int allocateFeatureVector_loop_24(int *i, int *descriptorDim, int *j, float *** *matrix, int *directionCount);
int allocate3D_loop_14(int *i, int *height, int *j, int *width, double *** *matrix, int *depth);
int generateKeypointSingle_loop_17(int *y, int *yMin, int *yMax, int *x, int *xMin, int *xMax, int *relativeX, KeyPoint * *point, int *relativeY, double *radius, ScaleSpace * *this, double *oneBinRad, double bins[], double *sigma);
int createDescriptors_loop_22(KeyPoint * *current, KeyPoint * *list, double *angle, const int *descriptorDim, const int *directionCount, int *y, int *radius, int *x, int *imageX, int *imageY, ScaleSpace * *this, double *yR, double *xR, double *dir, int *i, int *j, int *k, double *magnitudeWeight, double *sigma, int *iy, int *ix, int *id, double *binX, double *binY, double *binDir, double *xDist, double *xySpacing, double *yDist, double *dirDist, double *dirSpacing);
int capAndNormalizeFV_loop_28(int *i, KeyPoint * *this, int *j, int *k);

#endif
