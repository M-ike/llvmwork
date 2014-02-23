#include "imageio_loop_19.h"
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
	
    readPGMFile_loop_1(&i, number, &c, &imageFile);

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
	
    readPGMFile_loop_2(&i, number, &c, &imageFile);

	number[i] = '\0';
	height = atoi(number);

	while(isspace((c = getc(imageFile))))
		;
	
    readPGMFile_loop_3(&i, number, &c, &imageFile);

	number[i] = '\0';
	maxval = atoi(number);

	if(maxval > 255) {
		printf("***imageio.c/readPGMFile: The maximum grey value is greater than 255.\n");
		return NULL;
	}

	//allocate memory
	image = allocateImage(width, height);

	//read data
	
    readPGMFile_loop_4(&i, &image, &j, &value, &imageFile);

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
		{ int re_arg_pa1_5 = -1; 
    writePGMFile_loop_5(&i, &image, &j, &output, &maxival, &re_arg_pa1_5);
	if(re_arg_pa1_5 != -1) return; }


	fprintf(output, "P5\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	
    writePGMFile_loop_6(&i, &image, &j, &value, &maxival, &output);

	fclose(output);
}

void writePGMFileNegative(Image *image, FILE *output) {

	int i, j;
	float value;
	float maxival, minival, maxval = 0.0;

	/* find max and min pixel values to scale values to [0,1] */
	maxival = -1.0;
	minival = 1.0;
	
    writePGMFileNegative_loop_7(&i, &image, &j, &maxival, &minival);


	if(fabs(maxival) > fabs(minival))
		maxval = fabs(maxival);
	else
		maxval = fabs(minival);

	fprintf(output, "P5\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	
    writePGMFileNegative_loop_8(&i, &image, &j, &value, &maxval, &output);

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
	
    readPPMFile_loop_9(&i, number, &c, &imageFile);

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
	
    readPPMFile_loop_10(&i, number, &c, &imageFile);

	number[i] = '\0';
	height = atoi(number);

	while(isspace((c = getc(imageFile))))
		;
	
    readPPMFile_loop_11(&i, number, &c, &imageFile);

	number[i] = '\0';
	maxval = atoi(number);

	if(maxval > 255) {
		printf("***imageio.c/readPPMFile: The maximum sample value is greater than 255.\n");
		return NULL;
	}

	//allocate memory
	image = allocateImageRGB(width, height);

	//read data
	
    readPPMFile_loop_12(&i, &image, &j, &value, &imageFile);

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
		{ int re_arg_pa1_13 = -1; 
    writePPMFile_loop_13(&i, &image, &j, &output, &maxival, &re_arg_pa1_13);
	if(re_arg_pa1_13 != -1) return; }


	fprintf(output, "P6\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	
    writePPMFile_loop_14(&i, &image, &j, &value, &maxival, &output);

	fclose(output);
}

void writePPMFileNegative(ImageRGB *image, FILE *output) {

	int i, j;
	float value;
	float maxival, minival, maxval = 0.0;

	/* find max and min pixel values to scale values to [0,1] */
	maxival = -1.0;
	minival = 1.0;
	
    writePPMFileNegative_loop_15(&i, &image, &j, &maxival, &minival);


	if(fabs(maxival) > fabs(minival))
		maxval = fabs(maxival);
	else
		maxval = fabs(minival);

	fprintf(output, "P6\n");
	fprintf(output, "%d %d\n", image->width, image->height);
	fprintf(output, "255\n");
	
    writePPMFileNegative_loop_16(&i, &image, &j, &value, &maxval, &output);

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
	
    readDATFile_loop_17(&i, &image, &j, &imageFile, &value);

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
	
    writeDATFile_loop_18(&i, &image, &j, &maxival);


	fprintf(output, "%d\n%d\n", image->height, image->width);
	
    writeDATFile_loop_19(&i, &image, &j, &value, &maxival, &output);

		fclose(output);
}