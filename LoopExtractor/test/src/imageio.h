/*
*  Jan Prokaj
*  May 17, 2005
*/

#ifndef IMAGEIO_H
#define IMAGEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "image.h"

Image *readPGMFile(const char *fileName);
void writePGMFile(Image *image, const char *fileName);
void writePGMFileNegative(Image *image, FILE *output);

ImageRGB *readPPMFile(const char *fileName);
void writePPMFile(ImageRGB *image, const char *fileName);
void writePPMFileNegative(ImageRGB *image, FILE *output);

Image *readDATFile(const char *fileName);
void writeDATFile(Image *image, const char *fileName);

#endif
