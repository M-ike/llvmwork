/*
*  Jan Prokaj
*  May 11, 2005
*  based on ImageMap.cs -- Sebastian Nowozin (nowozin@cs.tu-berlin.de)
*/
#include "image.h"

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
	for(i = 0; i < new->height; ++i) {
		new->pic[i] = (float *) malloc(new->width*sizeof(float));
		if(new->pic[i] == NULL) {
			for(j = 0; j < i; ++j)
				free(new->pic[i]);
			free(new->pic);
			free(new);
			printf("***image.c/allocateImage: malloc() returned NULL");
			return NULL;
		}
	}
	
	/* initialize */
	for(i = 0; i < new->height; ++i)
		for(j = 0; j < new->width; ++j)
			new->pic[i][j] = 0.0;
	return new;
}

Image *allocateImageFromDoubles(double **array, int width, int height) {

	Image *new;
	int   i, j;

	new = allocateImage(width, height);
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			new->pic[i][j] = (float) array[i][j];
		}
	}
	return new;
}

//since image uses floats, we must divide by maxValue (such as 255)
Image *allocateImageFromInts(int **array, int maxValue, int width, int height) {

	Image *new;
	int   i, j;

	new = allocateImage(width, height);
	for(i = 0; i < new->height; ++i) {
		for(j = 0; j < new->width; ++j) {
			new->pic[i][j] = (float) array[i][j] / maxValue;
		}
	}
	return new;
}

void freeImage(Image *image) {

	int i;
	
	for(i = 0; i < image->height; ++i)
		free(image->pic[i]);
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
	for(i = 0 ; i < halved->height; ++i) {
		for(j = 0 ; j < halved->width; ++j) {
			halved->pic[i][j] = original->pic[2*i][2*j];
		}
	}
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
	for(i = 0 ; i < original->height; ++i) {
		for (j = 0 ; j < original->width; ++j) {

			/* last row/column */
			if(i == original->height-1 && j == original->width-1) {
				doubled->pic[2*i + 0][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 0][2*j + 1] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 1] = original->pic[i][j];
			}
			else if(i == original->height - 1) {
				doubled->pic[2*i + 0][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 0][2*j + 1] = 0.5*(original->pic[i][j] + original->pic[i][j+1]);
				doubled->pic[2*i + 1][2*j + 1] = doubled->pic[2*i + 0][2*j + 1];
			}
			else if(j == original->width - 1) {
				doubled->pic[2*i + 0][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 0][2*j + 1] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 0] = 0.5*(original->pic[i][j] + original->pic[i+1][j]);
				doubled->pic[2*i + 1][2*j + 1] = doubled->pic[2*i + 1][2*j + 0];
			}
			else {
				// pixel layout:
				// A B
				// C D

				// A
				doubled->pic[2*i + 0][2*j + 0] = original->pic[i][j];
				// B
				doubled->pic[2*i + 0][2*j + 1] = (original->pic[i][j] +
								original->pic[i][j+1]) / 2.0f;
				// C
				doubled->pic[2*i + 1][2*j + 0] = (original->pic[i][j] +
								original->pic[i+1][j]) / 2.0f;
				// D
				doubled->pic[2*i + 1][2*j + 1] = (original->pic[i][j] +
								original->pic[i][j+1] +
								original->pic[i+1][j] +
								original->pic[i+1][j+1]) / 4.0f;
			}
		}
	}
	
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
	for(i = 0; i < doubled->height; ++i) {
		doubled->pic[i] = (float *) malloc(doubled->width*sizeof(float));
	}
	
	//copy exisiting pixels
	for(i = 0 ; i < original->height; ++i) {
		for (j = 0 ; j < original->width; ++j) {
			doubled->pic[2*i][2*j] = original->pic[i][j];
		}
	}

	// fill new pixels
	for(i = 0 ; i < original->height; ++i) {
		for (j = 0 ; j < original->width; ++j) {
			
			/* last row/column */
			if(i == original->height-1 && j == original->width-1) {
				doubled->pic[2*i + 0][2*j + 1] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 1] = original->pic[i][j];
			}
			else if(i == original->height - 1) {
				doubled->pic[2*i + 1][2*j + 0] = original->pic[i][j];
				doubled->pic[2*i + 0][2*j + 1] = 0.5*(original->pic[i][j] + original->pic[i][j+1]);
				doubled->pic[2*i + 1][2*j + 1] = doubled->pic[2*i + 0][2*j + 1];
			}
			else if(j == original->width - 1) {
				doubled->pic[2*i + 0][2*j + 1] = original->pic[i][j];
				doubled->pic[2*i + 1][2*j + 0] = 0.5*(original->pic[i][j] + original->pic[i+1][j]);
				doubled->pic[2*i + 1][2*j + 1] = doubled->pic[2*i + 1][2*j + 0];
			}
			else {
				// pixel layout:
				// A B
				// C D

				// B
				if(doubled->pic[2*i][2*j] == 1.0 && doubled->pic[2*i][2*(j+1)] == 1.0) {
					doubled->pic[2*i][2*j + 1] = 1.0;
				}
				else {
					doubled->pic[2*i][2*j + 1] = 0.0;
				}
				
				// C
				if(doubled->pic[2*i][2*j] == 1.0 && doubled->pic[2*(i+1)][2*j] == 1.0) {
					doubled->pic[2*i + 1][2*j] = 1.0;
				}
				else {
					doubled->pic[2*i + 1][2*j] = 0.0;
				}

				// D
				if((doubled->pic[2*i][2*(j+1)] == 1.0 && doubled->pic[2*(i+1)][2*j]) || (doubled->pic[2*i][2*j] == 1.0 && doubled->pic[2*(i+1)][2*(j+1)] == 1.0)) {
					doubled->pic[2*i + 1][2*j + 1] = 1.0;
				}
				else {
					doubled->pic[2*i + 1][2*j + 1] = 0.0;
				}
			}
		}
	}
	
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
	for(i = 0; i < one->height; ++i)
		for(j = 0; j < one->width; ++j)
			newImage->pic[i][j] = one->pic[i][j] - two->pic[i][j];
		
	return newImage;
}

Image *cloneImage(Image *original) {

	Image *newImage;
	int i, j;

	/* allocate space */
	newImage = allocateImage(original->width, original->height);
	
	/* copy values */
	for(i = 0; i < newImage->height; ++i)
		for(j = 0; j < newImage->width; ++j)
			newImage->pic[i][j] = original->pic[i][j];
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
	for(i = 0; i < original->height; ++i) {
		imageX[i] = (float *) malloc(original->width*sizeof(float));
	}

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
	
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			sum1 = 0.0;
			if(j == 0) {
				sum1 = original->pic[i][j+1] - original->pic[i][j];
			}
			else if(j == original->width - 1) {
				sum1 = original->pic[i][j] - original->pic[i][j-1];
			}
			else {
				for(p = 0; p <= 2; ++p) {
					sum1 += original->pic[i][j-1+p]*maskX[p];
				}
			}
			imageX[i][j] = sum1;
		}
	}
	
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
	for(i = 0; i < original->height; ++i) {
		imageY[i] = (float *) malloc(original->width*sizeof(float));
	}

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
	
	for(j = 0; j < original->width; ++j) {
		for(i = 0; i < original->height; ++i) {
			sum2 = 0.0;
			if(i == 0) {
				sum2 = original->pic[i][j] - original->pic[i+1][j];
			}
			else if(i == original->height - 1) {
				sum2 = original->pic[i-1][j] - original->pic[i][j];
			}
			else {
				for(p = 0; p <= 2; ++p) {
					sum2 += original->pic[i-1+p][j]*maskY[p];
				}
			}
			imageY[i][j] = sum2;
		}
	}

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
	for(i = 0; i < original->height; ++i) {
		for(j = 0; j < original->width; ++j) {
			newImage->pic[i][j] = (float) sqrt(imageX[i][j]*imageX[i][j] + imageY[i][j]*imageY[i][j]);
		}
	}

	/* deallocate storage */
	for(i = 0; i < original->height; ++i) {
		free(imageX[i]);
		free(imageY[i]);
	}
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
	for(i = y, j = x; i < image->height && i > 0 && j < image->width && j > 0; i += yIncr, j += xIncr) {
		if(peakVal < image->pic[i][j])
			return 0;
		if(peakVal > image->pic[i][j]) {
			peakR = 1;
			break;
		}
	}
	if(!peakR)
		return 0;
	//iterates if equal value
	for(i = y, j = x; i < image->height && i > 0 && j < image->width && j > 0; i -= yIncr, j -= xIncr) {
		if(peakVal < image->pic[i][j])
			return 0;
		if(peakVal > image->pic[i][j]) {
			peakL = 1;
			break;
		}
	}

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
		for(i = x1; i <= x2; ++i) {
			comp = y1 + (i - x1) * dy / dx;
			if(comp >= 0 && comp < image->height &&
						i >= 0 && i < image->width)
				image->pic[comp][i] = 1.0f;
		}
	}
	else {
		/* sort points by y-axis */
		if(y1 > y2) {
			temp = y1; y1 = y2; y2 = temp;
			temp = x1; x1 = x2; x2 = temp;
		}
		dy = y2 - y1;
		dx = x2 - x1;
		for(i = y1; i <= y2; ++i) {
			comp = x1 + (i - y1) * dx / dy;
			if(i >= 0 && i < image->height &&
					comp >= 0 && comp < image->width)
				image->pic[i][comp] = 1.0f;
		}
	}
}

//export image data as x,y,z for plotting
void exportCSV(Image *image, char *fileName) {

	int i, j;
	FILE *output;

	output = fopen(fileName, "w");
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			fprintf(output, "%d,%d,%.4f\n", j, i, image->pic[i][j]);
		}
	}
	fclose(output);
}

float findMin(Image *image) {

	float min = 1.5;
	int   i, j;

	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->pic[i][j] < min)
				min = image->pic[i][j];
		}
	}
	return min;
}

float findMax(Image *image) {

	float max = -1.5;
	int   i, j;

	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			if(image->pic[i][j] > max)
				max = image->pic[i][j];
		}
	}
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
	for(i = 0; i < image->height; ++i) {
		for(j = 0; j < image->width; ++j) {
			new->red->pic[i][j] = image->pic[i][j];
			new->green->pic[i][j] = image->pic[i][j];
			new->blue->pic[i][j] = image->pic[i][j];
		}
	}
	return new;
}
