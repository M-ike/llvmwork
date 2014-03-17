/*
*  Jan Prokaj
*  May 11, 2005
*/
#ifndef CANNY_H
#define CANNY_H

#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "gaussconv.h"
 
Image *getCannyEdgesClassic(Image *original, float sigma, float percent);

Image *getCannyEdges(Image *image, float sigma, float percent, Image *angleSource);

void traceRidge(Image *peaks, Image *canny, int i, int j);

/************************************************
   Jason's Canny code ported from Java
      -- needed for a precise port of HeadFinder
*************************************************/

struct Canny {
	int    width, height; //image dimensions
	
	double **magnitude; //stores the magnitudes
	double **theta; //stores the angles
	
	Image  *edges; //final output
	
	double sigma;
};
typedef struct Canny Canny;

Canny  *newCannyPGM(Image *image, double sigma);

Canny  *newCannyPPM(ImageRGB *image, double sigma);

Canny  *newCannyPGMTwoSources(Image *gradient, Image *gaussian);

void   findThresholds(Canny *this, double *low, double *high);

void   traceColor(Canny *this, int x, int y, double lowThreshold, int **traced, int **peaks);

int    sectorAngle(Canny *this, int x, int y);

double max3(double d1, double d2, double d3);

//int **getEdges

int    isEdge(Canny *this, int x, int y);

void freeCanny(Canny *this);

#endif
