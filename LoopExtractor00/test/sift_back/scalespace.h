/*
*  Jan Prokaj
*  March 5, 2005
*/

#ifndef SCALESPACE_H
#define SCALESPACE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "siftconstants.h"
#include "image.h"
#include "gaussconv.h"
#include "canny.h"
#include "sort.h"

/* represents one octave of scale space */
struct ScaleSpace {
	double *relativeScaleTable; /* stores scale values within this space */

	Image  **images; /* array of pointers to images */
	int    imageCount; /* size of the array */

	int    imgHeight; /* height of images in this space */
	int    imgWidth; /* width of images in this space */
	double imageScale; /* scale relative to the original image (mult. of 2) */

	int    **processedLocalization; /* saves localized points to avoid duplicates */
	double ***magnitudes; /* magnitudes for each point in each level */
	double ***directions; /* directions for each point in each level */
};
typedef struct ScaleSpace ScaleSpace;

void printSpaceImages(ScaleSpace *this, const char *label);

ScaleSpace *buildScaleSpaceOctave(Image *original, double imgScale, double imageSigma);

ScaleSpace *buildDifferences(ScaleSpace *original);

ScaleSpace *buildScaleSpaceEdges(ScaleSpace *original);

ScaleSpace *buildScaleSpaceCanny(ScaleSpace *gaussian, ScaleSpace *derivative);

Image *getLastImage(ScaleSpace *this);

struct KeyPoint {
	int    minMax; /* 0 if min, 1 if max */

	int    x, y; /* location */
	double finalX, finalY; /* sub-pixel location */

	int    imageLevel; /* DoG level -- image index in images array */
	double imgScale, scale;
	double relativeScale;

	double orientation;

	float  ***featureVector; /* descriptor */
	int    xDim, yDim, orientationDim; /* dimensions of the descriptor */

	char   description[128]; /* used with output */

	int    count; /* only valid for the head of the list */
	struct KeyPoint *next; /* for building a list */
	struct KeyPoint *listRear; /* only valid for the head of the list */
};
typedef struct KeyPoint KeyPoint;

KeyPoint *findPeaks(ScaleSpace *this, Image *current, Image *above, Image *below, int level);

KeyPoint *linkPoint(KeyPoint *head, KeyPoint *new);

void printHistogram(ScaleSpace *this, KeyPoint *point);

KeyPoint* trash; //list of trashed points -- for debugging

FILE *histOutput; //only informational

FILE *histOutput2; //only informational

void getMagnitudesAndOrientations(ScaleSpace *this);

double ***allocate3D(int height, int width, int depth);

void deallocate3D(double ***matrix, int height, int width);

KeyPoint *generateKeypoints(ScaleSpace *this, KeyPoint *peaks);

KeyPoint *generateKeypointSingle(ScaleSpace *this, KeyPoint *list, KeyPoint *point, int *extraCount);

int interpolateOrientation(double left, double middle, double right, double *degreeCorrection, double *peakValue);

void createDescriptors(ScaleSpace *this, KeyPoint *list);

float ***allocateFeatureVector(int descriptorDim, int directionCount);

void capAndNormalizeFV(KeyPoint *this);

void printKeyPoint(KeyPoint *this, FILE *out);

void markImage(KeyPoint *list, Image* image);

void markImageSPoint(KeyPoint *list, Image* image, double scale);

#endif
