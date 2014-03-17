/*
*  Jan Prokaj
*  February 13, 2005
*/
#include "imageio.h"

//Reads image data from file, and returns Image via a pointer
//Returns NULL, if anything goes wrong
//Does not handle comments in PGM file!
Image *readPGMFile(const char *fileName) {

	int  c, value; //for IO
	int  i, j; //loop counters
	int  width, height, maxval; //pgm properties
	char number[6]; //should not have more than 5 digits
	Image *image;
	FILE *imageFile;

	imageFile = fopen(fileName, "rb");
	if(!imageFile) {
		printf("***imageio.c/readPGMFile: cannot open file %s\n", fileName);
		return NULL;
	}
	
	c = getc(imageFile);
	if(!(c == 'P' && getc(imageFile) == '5')) {
		printf("***imageio.c/readPGMFile: input file is not in PGM file format.\n");
		return NULL;
	}

	while(isspace((c = getc(imageFile)))) /* skip whitespace */
		;
	while(c == '#') { /* skip comments */
		while((c = getc(imageFile)) != '\n')
			;
		c = getc(imageFile);
	}
	/* save width */
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	width = atoi(number);

	while(isspace((c = getc(imageFile)))) /* skip whitespace */
		;
	while(c == '#') { /* skip comments */
		while((c = getc(imageFile)) != '\n')
			;
		c = getc(imageFile);
	}
	/* save height */
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	height = atoi(number);

	while(isspace((c = getc(imageFile))))
		;
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	maxval = atoi(number);

	if(maxval > 255) {
		printf("***imageio.c/readPGMFile: The maximum grey value is greater than 255.\n");
		return NULL;
	}

	//allocate memory
	image = allocateImage(width, height);

	//read data
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = getc(imageFile);
			value &=  0377;
			image->pic[i][j] = (float) ((float) value / 255.0f);
		}
	fclose(imageFile);
	return image;
}

//Writes image to a PGM file. The maximum grey value is
//fixed to 255
void writePGMFile(Image *image, const char *fileName) {

	int i, j;
	float value;
	float maxival;
	FILE *output;

	output = fopen(fileName, "wb");
	if(!output) {
		printf("***imageio.c/writePGMFile: cannot write to file %s."
		       "Check write permissions.", fileName);
		       return;
	}
	
	/* find max pixel value to scale down values to [0,1] */
	maxival = 0.0;
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->pic[i][j] < 0) {
				writePGMFileNegative(image, output);
				fclose(output);
				return;
			}
			if (image->pic[i][j] > maxival)
				maxival = image->pic[i][j];
		}
	}

	fprintf(output, "P5\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = image->pic[i][j] / maxival;
			value *= 255;
			fprintf(output, "%c", (char) ((int) value));
		}
	fclose(output);
}

void writePGMFileNegative(Image *image, FILE *output) {

	int i, j;
	float value;
	float maxival, minival, maxval = 0.0;

	/* find max and min pixel values to scale values to [0,1] */
	maxival = -1.0;
	minival = 1.0;
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->pic[i][j] > maxival)
				maxival = image->pic[i][j];
			if(image->pic[i][j] < minival)
				minival = image->pic[i][j];
		}
	}

	if(fabs(maxival) > fabs(minival))
		maxval = fabs(maxival);
	else
		maxval = fabs(minival);

	fprintf(output, "P5\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = image->pic[i][j] / maxval + 1.0;
			value *= 127;
			fprintf(output, "%c", (char) ((int) value));
		}
}

ImageRGB *readPPMFile(const char *fileName) {

	int  c, value; //for IO
	int  i, j; //loop counters
	int  width, height, maxval; //pgm properties
	char number[6]; //should not have more than 5 digits
	ImageRGB *image;
	FILE *imageFile;

	imageFile = fopen(fileName, "rb");
	if(!imageFile) {
		printf("***imageio.c/readPPMFile: cannot open file %s\n", fileName);
		return NULL;
	}
	
	c = getc(imageFile);
	if(!(c == 'P' && getc(imageFile) == '6')) {
		printf("***imageio.c/readPPMFile: input file is not in PPM file format.\n");
		return NULL;
	}

	while(isspace((c = getc(imageFile)))) /* skip whitespace */
		;
	while(c == '#') { /* skip comments */
		while((c = getc(imageFile)) != '\n')
			;
		c = getc(imageFile);
	}
	/* save width */
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	width = atoi(number);

	while(isspace((c = getc(imageFile)))) /* skip whitespace */
		;
	while(c == '#') { /* skip comments */
		while((c = getc(imageFile)) != '\n')
			;
		c = getc(imageFile);
	}
	/* save height */
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	height = atoi(number);

	while(isspace((c = getc(imageFile))))
		;
	for(i = 1, number[0] = c; !isspace(c = getc(imageFile)); ++i) {
		number[i] = c;
	}
	number[i] = '\0';
	maxval = atoi(number);

	if(maxval > 255) {
		printf("***imageio.c/readPPMFile: The maximum sample value is greater than 255.\n");
		return NULL;
	}

	//allocate memory
	image = allocateImageRGB(width, height);

	//read data
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = getc(imageFile);
			value &=  0377;
			image->red->pic[i][j] = (float) ((float) value / 255.0f);
			
			value = getc(imageFile);
			value &=  0377;
			image->green->pic[i][j] = (float) ((float) value / 255.0f);
			
			value = getc(imageFile);
			value &=  0377;
			image->blue->pic[i][j] = (float) ((float) value / 255.0f);
		}
	fclose(imageFile);
	return image;
}

void writePPMFile(ImageRGB *image, const char *fileName) {

	int i, j;
	float value;
	float maxival;
	FILE *output;

	output = fopen(fileName, "wb");
	if(!output) {
		printf("***imageio.c/writePPMFile: cannot write to file %s."
		       "Check write permissions.", fileName);
		       return;
	}
	
	/* find max pixel value to scale down values to [0,1] */
	maxival = 0.0;
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->red->pic[i][j] < 0 ||
			   image->green->pic[i][j] < 0 ||
			   image->blue->pic[i][j] < 0) {
				writePPMFileNegative(image, output);
				fclose(output);
				return;
			}
			if(image->red->pic[i][j] > maxival)
				maxival = image->red->pic[i][j];
			if(image->green->pic[i][j] > maxival)
				maxival = image->green->pic[i][j];
			if(image->blue->pic[i][j] > maxival)
				maxival = image->blue->pic[i][j];
		}
	}

	fprintf(output, "P6\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = image->red->pic[i][j] / maxival;
			value *= 255;
			fprintf(output, "%c", (char) ((int) value));
			
			value = image->green->pic[i][j] / maxival;
			value *= 255;
			fprintf(output, "%c", (char) ((int) value));
			
			value = image->blue->pic[i][j] / maxival;
			value *= 255;
			fprintf(output, "%c", (char) ((int) value));
		}
	fclose(output);
}

void writePPMFileNegative(ImageRGB *image, FILE *output) {

	int i, j;
	float value;
	float maxival, minival, maxval = 0.0;

	/* find max and min pixel values to scale values to [0,1] */
	maxival = -1.0;
	minival = 1.0;
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->red->pic[i][j] > maxival)
				maxival = image->red->pic[i][j];
			if(image->green->pic[i][j] > maxival)
				maxival = image->green->pic[i][j];
			if(image->blue->pic[i][j] > maxival)
				maxival = image->blue->pic[i][j];
			
			if(image->red->pic[i][j] < minival)
				minival = image->red->pic[i][j];
			if(image->green->pic[i][j] < minival)
				minival = image->green->pic[i][j];
			if(image->blue->pic[i][j] < minival)
				minival = image->blue->pic[i][j];
		}
	}

	if(fabs(maxival) > fabs(minival))
		maxval = fabs(maxival);
	else
		maxval = fabs(minival);

	fprintf(output, "P6\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = image->red->pic[i][j] / maxval + 1.0;
			value *= 127;
			fprintf(output, "%c", (char) ((int) value));
			
			value = image->green->pic[i][j] / maxval + 1.0;
			value *= 127;
			fprintf(output, "%c", (char) ((int) value));
			
			value = image->blue->pic[i][j] / maxval + 1.0;
			value *= 127;
			fprintf(output, "%c", (char) ((int) value));
		}
}


/*******************************************************************************************************/
Image *readDATFile(const char *fileName) {
	int  value; //for IO
	int  i, j; //loop counters
	int  width, height; //pgm properties
	Image *image;
	FILE *imageFile;

	imageFile = fopen(fileName, "rb");
	if(!imageFile) {
		printf("***imageio.c/readDATFile: cannot open file %s\n", fileName);
		return NULL;
	}

	/* save width */
	fscanf(imageFile,"%d",&width);
	/* save height */
	fscanf(imageFile,"%d",&height);
	//allocate memory
	image = allocateImage(width, height);

	//read data
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			fscanf(imageFile,"%d",&value);
//			value &=  0377;
			image->pic[i][j] = (float) ((float) value / 255.0f);
		}
		fclose(imageFile);
		return image;
}

void writeDATFile(Image *image, const char *fileName) {

	int i, j;
	float value;
	float maxival;
	FILE *output;

	output = fopen(fileName, "wb");
	if(!output) {
		printf("***imageio.c/writeDATFile: cannot write to file %s."
			"Check write permissions.", fileName);
		return;
	}

	/* find max pixel value to scale down values to [0,1] */
	maxival = 0.0;
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
/*			if(image->pic[i][j] < 0) {
				writePGMFileNegative(image, output);
				fclose(output);
				return;
			}*/
			if (image->pic[i][j] > maxival)
				maxival = image->pic[i][j];
		}
	}

	fprintf(output, "%d\n%d\n", image->height, image->width);
	for(i = 0; i < image->height; ++i)
		for(j = 0; j < image->width; ++j) {
			value = image->pic[i][j] / maxival;
			value *= 255;
			fprintf(output, "%d\n", (int)value);
		}
		fclose(output);
}