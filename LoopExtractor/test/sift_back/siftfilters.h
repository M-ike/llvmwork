/*
*  Jan Prokaj
*  May 14, 2005
*/
#ifndef SIFTFILTERS_H
#define SIFTFILTERS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_linalg.h>
#include "image.h"
#include "scalespace.h"
#include "matrix.h"
#include "sort.h"

KeyPoint *filterAndLocalizePeaks(ScaleSpace *spaces, ScaleSpace *gaussian, KeyPoint *peaks);

int localizeIsWeak (ScaleSpace *this, KeyPoint *point);

Matrix *getAdjustment(ScaleSpace *this, KeyPoint *point, int level, double *dotP);

int isTooEdgeLike(Image *image, int x, int y, float *edgeRatio);

int isFingerTip(ScaleSpace *this, ScaleSpace *gaussian, KeyPoint *point);

#endif
