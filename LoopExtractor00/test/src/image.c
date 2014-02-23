#include "image_loop_26.h"
Image *allocateImage(int width, int height) {

	Image *new;
	int i, j;
	
	/* memory allocations */
	new = (Image *) malloc(sizeof(Image));
	if(new == NULL) {
		printf("***image.c/allocateImage: malloc() returned NULL");
		return NULL;
	}
	new->width = width;
	new->height = height;
	new->pic = (float **) malloc(new->height*sizeof(float *));
	if(new->pic == NULL) {
		free(new);
		printf("***image.c/allocateImage: malloc() returned NULL");
		return NULL;
	}
		{ int re_arg_pa1_1 = -1; Image * re_arg_pa2_1;
    allocateImage_loop_1(&i, &new, &j, &re_arg_pa1_1, &re_arg_pa2_1);
	if(re_arg_pa1_1 != -1) return re_arg_pa2_1; }

	
	/* initialize */
	
    allocateImage_loop_2(&i, &new, &j);

	return new;
}

Image *allocateImageFromDoubles(double **array, int width, int height) {

	Image *new;
	int   i, j;

	new = allocateImage(width, height);
	
    allocateImageFromDoubles_loop_3(&i, &new, &j, &array);

	return new;
}

//since image uses floats, we must divide by maxValue (such as 255)
Image *allocateImageFromInts(int **array, int maxValue, int width, int height) {

	Image *new;
	int   i, j;

	new = allocateImage(width, height);
	
    allocateImageFromInts_loop_4(&i, &new, &j, &array, &maxValue);

	return new;
}

void freeImage(Image *image) {

	int i;
	
	
    freeImage_loop_5(&i, &image);

	free(image->pic);
	free(image);
}

//Halve the size of an image.
//allocates storage and returns via pointer
//returns NULL if halving not possible (because pic is too small,
//or because malloc returns NULL
Image *halveImage(Image *original)
{
	Image *halved; //halved Image
	int i, j; //loop counters

	if((original->width / 2) == 0 || (original->height / 2) == 0)
		return NULL;

	/* initialization of the halved image */
	halved = allocateImage(original->width/2, original->height/2);
	
    halveImage_loop_6(&i, &halved, &j, &original);

	return halved;
}

// Double the size of an image using linear interpolation.
//allocates storage and returns via pointer
//returns NULL if doubling not possible (because pic is too small,
//or because malloc returns NULL
Image *doubleImage(Image *original)
{
	Image *doubled; //doubled Image
	int i, j; //loop counters

	// Doubling an image with x/y dimensions less or equal than 2 will
	// result in an image with just (2, 2) dims, so it's useless.
	if (original->width <= 2 || original->height <= 2) {
		printf("***image.c/doubleImage: Original image too small for doubling.\n");
		return NULL;
	}

	/* initialization of the doubled image */
	doubled = allocateImage(2*original->width, 2*original->height);

	// fill four pixels per step, except for the last row/col, which will
	// be filled with 0
	
    doubleImage_loop_7(&i, &original, &j, &doubled);

	
	return doubled;
}

//factor must be a power of 2!
Image *resizeImage(Image *original, double factor) {

	Image *newImage, *temp;
	int exponent;
	int i, j;

	if(factor == 1) {
		return original;
	}
	else if(factor > 1) {
		exponent = (int)(0.5+log(factor)/log(2.0));
		newImage = original;
		for(i = 0; i < exponent; ++i) {
			temp = doubleImage(newImage);
			if(newImage != original) {
				//replace with freeImage(newImage);
				for(j = 0; j < newImage->height; ++j) {
					free(newImage->pic[j]);
				}
				free(newImage->pic);
				free(newImage);
			}
			newImage = temp;
		}
	}
	else if(factor < 1){
		exponent = (int)(0.5+log(1/factor)/log(2.0));
		newImage = original;
		for(i = 0; i < exponent; ++i) {
			temp = halveImage(newImage);
			if(newImage != original) {
				//replace with freeImage(newImage)
				for(j = 0; j < newImage->height; ++j) {
					free(newImage->pic[j]);
				}
				free(newImage->pic);
				free(newImage);
			}
			newImage = temp;
		}
	}

	return newImage;
}

Image *doubleCanny(Image *original) {

	Image *doubled; //doubled Image
	int i, j; //loop counters

	// Doubling an image with x/y dimensions less or equal than 2 will
	// result in an image with just (2, 2) dims, so it's useless.
	if (original->width <= 2 || original->height <= 2) {
		printf("***image.c/doubleImage: Original image too small for doubling.\n");
		return NULL;
	}

	/* initialization of the doubled image */
	doubled = (Image *) malloc(sizeof(Image));
	if(!doubled) {
		printf("***image.c/doubleImage: Allocating a new image failed (malloc).\n");
		return NULL;
	}
	doubled->width = 2*original->width;
	doubled->height = 2*original->height;
	doubled->pic = (float **) malloc(doubled->height*sizeof(float *));
	
    doubleCanny_loop_8(&i, &doubled);

	
	//copy exisiting pixels
	
    doubleCanny_loop_9(&i, &original, &j, &doubled);


	// fill new pixels
	
    doubleCanny_loop_10(&i, &original, &j, &doubled);

	
	return doubled;
}

Image *resizeCanny(Image *original, double factor) {

	Image *newImage, *temp;
	int exponent;
	int i, j;

	if(factor == 1) {
		return original;
	}
	else if(factor > 1) {
		exponent = (int)(0.5+log(factor)/log(2.0));
		newImage = original;
		for(i = 0; i < exponent; ++i) {
			temp = doubleCanny(newImage);
			if(newImage != original) {
				for(j = 0; j < newImage->height; ++j) {
					free(newImage->pic[j]);
				}
				free(newImage->pic);
				free(newImage);
			}
			newImage = temp;
		}
	}
	else if(factor < 1){
		exponent = (int)(0.5+log(1/factor)/log(2.0));
		newImage = original;
		for(i = 0; i < exponent; ++i) {
			temp = halveImage(newImage);
			if(newImage != original) {
				for(j = 0; j < newImage->height; ++j) {
					free(newImage->pic[j]);
				}
				free(newImage->pic);
				free(newImage);
			}
			newImage = temp;
		}
	}

	return newImage;
}

//Subtracts image two from image one -- 1-2
//Images have to be the same size
Image *subtractImages(Image *one, Image *two) {

	int i, j;
	Image *newImage;

	if((one->width != two->width) || (one->height != two->height))
		return NULL;

	/* allocation of the subtracted image */
	newImage = allocateImage(one->width, one->height);

	/* subtract */
	
    subtractImages_loop_11(&i, &one, &j, &newImage, &two);

		
	return newImage;
}

Image *cloneImage(Image *original) {

	Image *newImage;
	int i, j;

	/* allocate space */
	newImage = allocateImage(original->width, original->height);
	
	/* copy values */
	
    cloneImage_loop_12(&i, &newImage, &j, &original);

	return newImage;
}

float** getDerivativeX(Image *original) {

	/* OLD ROBERTS
	int maskX[2][2] = {{-1,0},
	                    {0,1}};*/
	int maskX[3] = {-1,0,1};
	int i, j, p, q;
	double sum1;
	float **imageX;

	/* allocate space for x component */
	imageX = (float **) malloc(original->height*sizeof(float *));
	
    getDerivativeX_loop_13(&i, &original, &imageX);


	/* convolve OLD ROBERTS
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			imageX[i][j] = 0.0;
			if(i == original->height - 1 || j == original->width - 1)
				continue;
			sum1 = 0.0;
			for(p = 0; p <= 1; ++p) {
				for(q = 0; q <= 1; ++q) {
					sum1 += original->pic[i+p][j+q]*maskX[p][q];
				}
			}
			imageX[i][j] = sum1;
		}
	}*/
	
	
    getDerivativeX_loop_14(&i, &original, &j, &sum1, &p, maskX, &imageX);

	
	return imageX;
}

float** getDerivativeY(Image *original) {

	/* OLD ROBERTS
	int maskY[2][2] = {{0,-1},
	                   {1,0}};*/
	int maskY[3] = {1,0,-1};
	int i, j, p, q;
	double sum2;
	float **imageY;

	/* allocate space for y component */
	imageY = (float **) malloc(original->height*sizeof(float *));
	
    getDerivativeY_loop_15(&i, &original, &imageY);


	/* convolve OLD ROBERTS
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			imageY[i][j] = 0.0;
			if(i == original->height - 1 || j == original->width - 1)
				continue;
			sum2 = 0.0;
			for(p = 0; p <= 1; ++p) {
				for(q = 0; q <= 1; ++q) {
					sum2 += original->pic[i+p][j+q]*maskY[p][q];
				}
			}
			imageY[i][j] = sum2;
		}
	}*/
	
	
    getDerivativeY_loop_16(&j, &original, &i, &sum2, &p, maskY, &imageY);


	return imageY;
}

//run a simple derivative mask, roberts cross
Image *getDerivative(Image *original) {

	int i, j;
	float **imageX;
	float **imageY;
	Image *newImage;

	/* allocate new image */
	newImage = allocateImage(original->width, original->height);

	imageX = getDerivativeX(original);
	imageY = getDerivativeY(original);

	/* compute gradient */
	
    getDerivative_loop_17(&i, &original, &j, &newImage, &imageX, &imageY);


	/* deallocate storage */
	
    getDerivative_loop_18(&i, &original, &imageX, &imageY);

	free(imageX);
	free(imageY);

	return newImage;
}

//direction0 = horizontal -
//direction1 = diagonal /
//direction2 = diagonal \
//direction3 = vertical |
int isPeak(Image *image, int x, int y, int direction) {

	int peakL = 0;
	int peakR = 0;
	float peakVal = image->pic[y][x];
	int i, j;
	int yIncr, xIncr;

	if(direction == 0) { //horizontal
		yIncr = 0;
		xIncr = 1;
	}
	else if(direction == 1) { //diagonal /
		yIncr = -1;
		xIncr = 1;
	}
	else if(direction == 2) { //diagonal \
		yIncr = 1;
		xIncr = 1;
	}
	else if(direction == 3) { //vertical
		yIncr = 1;
		xIncr = 0;
	}
	else
		return 0; //error

	//iterates if equal value
		{ int re_arg_pa1_19 = -1; int re_arg_pa2_19;
    isPeak_loop_19(&i, &y, &j, &x, &image, &yIncr, &xIncr, &peakVal, &peakR, &re_arg_pa1_19, &re_arg_pa2_19);
	if(re_arg_pa1_19 != -1) return re_arg_pa2_19; }

	if(!peakR)
		return 0;
	//iterates if equal value
		{ int re_arg_pa1_20 = -1; int re_arg_pa2_20;
    isPeak_loop_20(&i, &y, &j, &x, &image, &yIncr, &xIncr, &peakVal, &peakL, &re_arg_pa1_20, &re_arg_pa2_20);
	if(re_arg_pa1_20 != -1) return re_arg_pa2_20; }


	return peakL;
}

/* based on Lowe's line drawing code */
void drawLine(Image *image, int y1, int x1, int y2, int x2) {

	int i, dy, dx, temp;
	int comp;

	if(y1 == y2 && x1 == x2) /* line of zero length */
		return;

	/* Is line more horizontal than vertical? */
	if(ABS(y2 - y1) < ABS(x2 - x1)) {

		/* sort points by x-axis */
		if(x1 > x2) {
			temp = y1; y1 = y2; y2 = temp;
			temp = x1; x1 = x2; x2 = temp;
		}
		dy = y2 - y1;
		dx = x2 - x1;
		
    drawLine_loop_21(&i, &x1, &x2, &comp, &y1, &dy, &dx, &image);

	}
	else {
		/* sort points by y-axis */
		if(y1 > y2) {
			temp = y1; y1 = y2; y2 = temp;
			temp = x1; x1 = x2; x2 = temp;
		}
		dy = y2 - y1;
		dx = x2 - x1;
		
    drawLine_loop_22(&i, &y1, &y2, &comp, &x1, &dx, &dy, &image);

	}
}

//export image data as x,y,z for plotting
void exportCSV(Image *image, char *fileName) {

	int i, j;
	FILE *output;

	output = fopen(fileName, "w");
	
    exportCSV_loop_23(&i, &image, &j, &output);

	fclose(output);
}

float findMin(Image *image) {

	float min = 1.5;
	int   i, j;

	
    findMin_loop_24(&i, &image, &j, &min);

	return min;
}

float findMax(Image *image) {

	float max = -1.5;
	int   i, j;

	
    findMax_loop_25(&i, &image, &j, &max);

	return max;
}

/******************************************
	ImageRGB methods
******************************************/

ImageRGB *allocateImageRGB(int width, int height) {

	ImageRGB *new;
	Image *redChannel;
	Image *greenChannel;
	Image *blueChannel;
	
	/* memory allocations */
	new = (ImageRGB *) malloc(sizeof(ImageRGB));
	if(new == NULL) {
		printf("***image.c/allocateImageRGB: malloc() returned NULL");
		return NULL;
	}
	new->width = width;
	new->height = height;
	
	redChannel = allocateImage(width, height);
	greenChannel = allocateImage(width, height);
	blueChannel = allocateImage(width, height);
	
	new->red = redChannel;
	new->green = greenChannel;
	new->blue = blueChannel;
	
	return new;
}

void freeImageRGB(ImageRGB *image) {

	freeImage(image->red);
	freeImage(image->green);
	freeImage(image->blue);
	free(image);
}

ImageRGB *convertToRGB(Image *image) {

	ImageRGB *new;
	int i, j;
	
	new = allocateImageRGB(image->width, image->height);
	
    convertToRGB_loop_26(&i, &image, &j, &new);

	return new;
}
