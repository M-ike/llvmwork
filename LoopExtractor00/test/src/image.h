/*
*  Jan Prokaj
*  April 8, 2005
*/

#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h> /*for malloc/free */
#include <math.h>
#include "siftconstants.h"

#define ABS(x)    (((x) > 0) ? (x) : (-(x)))

struct Image {
	int   width;
	int   height;
	float **pic; //values between [0,1]
};
typedef struct Image Image;

Image *allocateImage(int width, int height);

Image *allocateImageFromDoubles(double **array, int width, int height);

Image *allocateImageFromInts(int **array, int maxValue, int width, int height);

void freeImage(Image *image);

Image *doubleImage (Image *original);

Image *halveImage(Image *original);

Image *resizeImage(Image *original, double factor);

Image *doubleCanny(Image *original);

Image *resizeCanny(Image *original, double factor);

Image *subtractImages(Image *one, Image *two);

Image *cloneImage(Image *original);

float** getDerivativeY(Image *original);

float** getDerivativeX(Image *original);

Image *getDerivative(Image *original);

int isPeak(Image *image, int x, int y, int direction);

void drawLine(Image *image, int y1, int x1, int y2, int x2);

void exportCSV(Image *image, char *fileName);

float findMax(Image *image);

float findMin(Image *image);

struct ImageRGB {
	int width;
	int height;
	Image *red;
	Image *green;
	Image *blue;
};
typedef struct ImageRGB ImageRGB;

ImageRGB *allocateImageRGB(int width, int height);

void freeImageRGB(ImageRGB *image);

ImageRGB *convertToRGB(Image *image);

#endif
