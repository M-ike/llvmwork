/*
*  Jan Prokaj
*  October 29, 2004
*/

#ifndef GAUSSCONV_H
#define GAUSSCONV_H

#include <math.h>
#include <stdlib.h>
#include "image.h"

Image* gaussianD1Convolution2D(Image *original, double sigma);

Image* gaussianConvolution2D(Image *original, double sigma);

Image* gaussianConvolution2DFast(Image *original, double sigma);

#endif
