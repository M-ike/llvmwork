/*
*  Jan Prokaj
*  May 28, 2005
*/

#ifndef OCTAVE_H
#define OCTAVE_H

#include <stdio.h>
#include <stdlib.h>
#include "scalespace.h"
#include "image.h"
#include "siftconstants.h"
#include "gaussconv.h"

struct Octave {
	ScaleSpace *gaussianSpace;
	ScaleSpace *diffGaussianSpace;
	struct Octave *next;
};
typedef struct Octave Octave;

Octave *link(Octave *head, Octave *new);

KeyPoint *getPeaks(Octave *this);

//Octave *buildScaleSpacePyramidSIFT(Image *image);

Octave *buildScaleSpacePyramid(Image *image); //returns a linked list of octaves

#endif
