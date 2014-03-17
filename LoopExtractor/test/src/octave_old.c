/*
*  Jan Prokaj
*  October 31, 2004
*/
#include "octave.h"

//adds an element to the front of the linked list
//returns the new head of the list
Octave *link(Octave *head, Octave *new) {

	/* first item */
	if(head == NULL) {
		new->next = NULL;
		return new;
	}
	new->next = head;
	return new;
}

//loops over the octave and looks for peaks
//the octave has to have at least 3 images
KeyPoint *getPeaks(Octave *this) {

	ScaleSpace *space;
	Image      *current, *above, *below;
	int        level;
	KeyPoint *peaks = NULL;
	KeyPoint *temp;

	space = this->diffGaussianSpace;
	if(space->imageCount < 3) {
		printf("***octave.c/getPeaks(): not enough images\n");
		return NULL;
	}
	level = 1;
	while((level+1) < space->imageCount) {
		below = space->images[level-1];
		current = space->images[level];
		above = space->images[level+1];
		temp = findPeaks(space, current, above, below, level);
		if(temp != NULL) {
			/* merge lists */
			if(peaks != NULL) {
				//printf("merging lists\n");
				peaks->listRear->next = temp;
				peaks->listRear = temp->listRear;
				peaks->count += temp->count;
				//printf("done merging\n");
			}
			else {
				//printf("new list\n");
				peaks = temp;
			}
		}
		++level;
	}
	return peaks;
}

//does not create diffGaussianSpace
Octave *buildScaleSpacePyramid(Image *image) {

	Image  *doubledImage;
	Image  *currentImage;
	double scale;
	double imageSigma;
	Octave *pyramid = NULL;
	Octave *temp;
	int    octaveCount = 0;

	scale = START_SCALE;
	
	/* double image to save data */
	doubledImage = doubleImage(image);
	if(doubledImage == NULL) {
		printf("***octave.c/buildScaleSpacePyramid(): doubling image failed\n");
		return NULL;
	}
	scale /= 2.0;
	printf("\nImage doubled\n");

	/* preprocessing smoothing*/
	printf("Preprocessing smoothing\n");
	doubledImage = gaussianConvolution2DFast(doubledImage, 1.0); //works

	/* build octaves of scalespace */
	printf("Octave pyramid construction\n");
	currentImage = doubledImage;
	while(octaveCount < OCTAVE_LIMIT && currentImage->width >= MIN_SIZE && currentImage->height >= MIN_SIZE) {
		if(currentImage == NULL) {
			printf("***octave.c/buildScaleSpacePyramid(): Current image null.\n");
			return NULL;
		}

		/* create octave pointer wrapper */
		printf("  Octave for pic %d %d (%d)\n", currentImage->width, currentImage->height, octaveCount);
		temp = (Octave *) malloc(sizeof(Octave));
		if(temp == NULL) {
			printf("***octave.c/buildScaleSpacePyramid(): Out of memory\n");
			return NULL;
		}
		
		/* build octave scale space L(x,y,sigma) */
		printf("    Gaussian Scale Space\n");
		if(octaveCount == 0)
			imageSigma = 1.0; //blur of the first image is 1.0
		else
			imageSigma = START_SIGMA;
		temp->gaussianSpace = buildScaleSpaceOctave(currentImage, scale, imageSigma);

		/* store and go on */
		pyramid = link(pyramid, temp);
		currentImage = halveImage(getLastImage(temp->gaussianSpace));
		scale *= 2.0;
		++octaveCount;
		printf("    Image halved\n");
	}
	--octaveCount; /* one extra */
	
	return pyramid;
}
