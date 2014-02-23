#include "scalespace_loop_33.h"
ScaleSpace *buildScaleSpaceOctave(Image *original, double imgScale, double imageSigma) {

	ScaleSpace *space;
	int   i;
	double sigma;
	double k; /* see paper */
	double sourceBlur, targetBlur;

	space = (ScaleSpace *) malloc(sizeof(ScaleSpace));
	if(space == NULL)
		return NULL;
	space->imageCount = IMAGES_PER_OCTAVE + 2;
	space->imageScale = imgScale;
	space->images = (Image **) malloc(space->imageCount*sizeof(Image *));
	if(space->images == NULL)
		return NULL;

	/* TODO: this should be allocated and computed only once --> all spaces are same */
	space->relativeScaleTable = (double *) malloc(space->imageCount*sizeof(double));

	k = pow(2.0, (double) (1.0/(IMAGES_PER_OCTAVE-1)));
	sigma = START_SIGMA;
	
    buildScaleSpaceOctave_loop_1(&i, &space, &sigma, &k);


	
    buildScaleSpaceOctave_loop_2(&i, &space, &sourceBlur, &imageSigma, &targetBlur, &original, &sigma);

	space->imgHeight = original->height;
	space->imgWidth = original->width;
	space->processedLocalization = NULL;
	return space;
}

//Builds the difference of Gaussian scale space
ScaleSpace *buildDifferences(ScaleSpace *original) {

	ScaleSpace *space;
	int   i, j;

	space = (ScaleSpace *) malloc(sizeof(ScaleSpace));
	if(space == NULL)
		return NULL;
	space->imageCount = original->imageCount - 1;
	space->imageScale = original->imageScale;
	space->imgHeight = original->imgHeight;
	space->imgWidth = original->imgWidth;
	space->relativeScaleTable = original->relativeScaleTable;
	space->images = (Image **) malloc(space->imageCount*sizeof(Image *));
	if(space->images == NULL)
		return NULL;
	
    buildDifferences_loop_3(&i, &space, &original);


	space->processedLocalization = (int **) malloc(space->imgHeight*sizeof(int *));
	
    buildDifferences_loop_4(&i, &space);

	
    buildDifferences_loop_5(&i, &space, &j);

	return space;
}

//Builds the edge scale space
ScaleSpace *buildScaleSpaceEdges(ScaleSpace *original) {

	ScaleSpace *space;
	int   i;

	space = (ScaleSpace *) malloc(sizeof(ScaleSpace));
	if(space == NULL)
		return NULL;
	space->imageCount = original->imageCount;
	space->imageScale = original->imageScale;
	space->imgHeight = original->imgHeight;
	space->imgWidth = original->imgWidth;
	space->relativeScaleTable = original->relativeScaleTable;
	space->images = (Image **) malloc(space->imageCount*sizeof(Image *));
	if(space->images == NULL)
		return NULL;
	
    buildScaleSpaceEdges_loop_6(&i, &space, &original);


	return space;
}

ScaleSpace *buildScaleSpaceCanny(ScaleSpace *gaussian, ScaleSpace *derivative) {

	ScaleSpace *space;
	int   i;

	space = (ScaleSpace *) malloc(sizeof(ScaleSpace));
	if(space == NULL)
		return NULL;
	space->imageCount = gaussian->imageCount;
	space->imageScale = gaussian->imageScale;
	space->imgHeight = gaussian->imgHeight;
	space->imgWidth = gaussian->imgWidth;
	space->relativeScaleTable = gaussian->relativeScaleTable;
	space->images = (Image **) malloc(space->imageCount*sizeof(Image *));
	if(space->images == NULL)
		return NULL;
	
    buildScaleSpaceCanny_loop_7(&i, &space, &derivative, &gaussian);


	return space;
}

//returns image with 2*sigma (two images from the top of the stack of an octave)
Image *getLastImage(ScaleSpace *this) {
	if(this->imageCount < 3)
		return NULL;
	return this->images[this->imageCount - 3];
}

//returns a list of peaks for the current image
//DoGlevel --> difference of gaussian image level
KeyPoint *findPeaks(ScaleSpace *this, Image *current, Image *above, Image *below, int DoGlevel) {

	KeyPoint *peaks = NULL;
	KeyPoint *temp;
	int   i, j; //loop counters
	int   p, q;
	float min, max;
	int   isMin;
	int   isMax;

	/* check if minimum or maximum */
	
    findPeaks_loop_8(&i, &current, &j, &min, &isMin, &p, &q, &above, &below, &temp, &DoGlevel, &this, &peaks, &max, &isMax);

	return peaks;
}

//adds an element to the front of the linked list
//returns the new head of the list
//updates a variable for the end of the list
KeyPoint *linkPoint(KeyPoint *head, KeyPoint *new) {

	/* first item */
	if(head == NULL) {
		new->next = NULL;
		new->listRear = new;
		new->count = 1;
		return new;
	}
	new->listRear = head->listRear;
	new->next = head;
	new->count = head->count + 1;
	return new;
}

//print images in the scalespace, filename is: label#
void printSpaceImages(ScaleSpace *this, const char *label) {

	int i;
	char fileName[128] = {'\0'};
	FILE *imageFile;
	int width, height;

	width = this->imgWidth;
	height = this->imgHeight;
	for(i = 0; i < this->imageCount; ++i) {
		sprintf(fileName, "%s%d.pgm", label, i);
		writePGMFile(this->images[i], fileName);
	}
}

//only used in debugging, not part of SIFT
void printHistogram(ScaleSpace *this, KeyPoint *point) {

	double   radius;
	int      x, y;
	int      xMin, xMax, yMin, yMax;
	double   bins[36] = {0.0}; //36 bins for 360 degrees
	int      binIndex;
	int      maxBin;
	int      relativeX, relativeY;
	double   sigma;
	double   maxGradient;
	int      left, right;
	double   oneBinRad;
	double   degree;
	int      sn, sw;
	double   firstE, last, cur, next;
	int      barWidth, i;
	double   direction, xDiff, yDiff, magnitude;

	sigma = 1.5*point->relativeScale; /* sigma = 1.5*keypoint scale*/
	radius = 3.0*sigma;

	xMin = ((point->x - radius) > 1) ? (point->x - radius) : 1;
	xMax = ((point->x + radius) < (this->images[0]->width - 1)) ? (point->x + radius) : (this->images[0]->width - 1);
	yMin = ((point->y - radius) > 1) ? (point->y - radius) : 1;
	yMax = ((point->y + radius) < (this->images[0]->height - 1)) ? (point->y + radius) : (this->images[0]->height - 1);

	oneBinRad = (2.0*M_PI) / 36.0;

	/* build the histogram */
	
    printHistogram_loop_9(&y, &yMin, &yMax, &x, &xMin, &xMax, &relativeX, &point, &relativeY, &radius, &yDiff, &this, &xDiff, &direction, &binIndex, &oneBinRad, &magnitude, bins, &sigma);


	/* find maximum peak for histogram output */
	maxGradient = 0.0;
	
    printHistogram_loop_10(&binIndex, bins, &maxGradient);


	/* output histogram */
	fprintf(histOutput2, "Pt. %d %d, Level %d\n", (int) (point->y), (int) (point->x), point->imageLevel);
	
    printHistogram_loop_11(&binIndex, &barWidth, bins, &maxGradient, &i);

	fprintf(histOutput2, "\n");
}

//depth is imageCount - 2, because we want to use the same index for the
//image the magnitudes/orientations belong to
//we ignore index 0
//Ex: if we have 6 original (L) images, peaks are going to have levels
//     1, 2, 3
//    we want to use these same numbers to select the proper L image
//borders are left uncalculated
//pg. 13
void getMagnitudesAndOrientations(ScaleSpace *this) {

	int i, j, k;
	int width, height, imageLevels;
	double xDiff, yDiff;

	width = this->imgWidth;
	height = this->imgHeight;
	imageLevels = this->imageCount - 2;
	
    getMagnitudesAndOrientations_loop_12(&i, &height, &j, &width, &k, &imageLevels, &xDiff, &this, &yDiff);

}

double ***allocate3D(int height, int width, int depth) {

	int i, j, k;
	double ***matrix;

	matrix = (double ***) malloc(height*sizeof(double **));
	if(matrix == NULL) {
		printf("malloc returned null\n");
		exit(1);
	}
	
    allocate3D_loop_13(&i, &height, &matrix, &width);

	
    allocate3D_loop_14(&i, &height, &j, &width, &matrix, &depth);

	return matrix;
}

void deallocate3D(double ***matrix, int height, int width) {

	int i, j;

	
    deallocate3D_loop_15(&i, &height, &j, &width, &matrix);

	free(matrix);
}

KeyPoint *generateKeypoints(ScaleSpace *this, KeyPoint *peaks) {

	KeyPoint *current;
	int      multipleKeypoints = 0;

	
    generateKeypoints_loop_16(&current, &peaks, &this, &multipleKeypoints);

	printf("      added %d keypoints with new orientation\n", multipleKeypoints);
	return peaks;
}

KeyPoint *generateKeypointSingle(ScaleSpace *this, KeyPoint *list, KeyPoint *point, int *extraCount) {

	double   radius;
	int      x, y;
	int      xMin, xMax, yMin, yMax;
	double   bins[36] = {0.0}; //36 bins for 360 degrees
	int      isKeypoint[36] = {0};
	int      binIndex, maxBin;
	int      relativeX, relativeY;
	double   sigma;
	double   maxGradient;
	double   maxDegreeCorrection;
	int      left, right;
	double   oneBinRad;
	double   degree;
	int      sn, sw;
	double   firstE, last, cur, next;
	KeyPoint *newKey = NULL;
	KeyPoint *lastAddition = NULL;
	int      barWidth, i;

	sigma = 1.5*point->relativeScale; /* sigma = 1.5*keypoint scale*/
	radius = 3.0*sigma;

	xMin = ((point->x - radius) > 1) ? (point->x - radius) : 1;
	xMax = ((point->x + radius) < (this->images[0]->width - 1)) ? (point->x + radius) : (this->images[0]->width - 1);
	yMin = ((point->y - radius) > 1) ? (point->y - radius) : 1;
	yMax = ((point->y + radius) < (this->images[0]->height - 1)) ? (point->y + radius) : (this->images[0]->height - 1);

	oneBinRad = (2.0*M_PI) / 36.0;

	/* build the histogram */
	
    generateKeypointSingle_loop_17(&y, &yMin, &yMax, &x, &xMin, &xMax, &relativeX, &point, &relativeY, &radius, &this, &oneBinRad, bins, &sigma);


	/* find maximum peak for histogram output */
	maxGradient = 0.0;
	
    generateKeypointSingle_loop_18(&binIndex, bins, &maxGradient);


	/* output histogram -- only informational
	fprintf(histOutput, "Pt. %d %d, Level %d\n", (int) (point->y*this->imageScale), (int) (point->x*this->imageScale), point->imageLevel);
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		fprintf(histOutput, "%2d. ", binIndex);
		barWidth = (int) (bins[binIndex] / maxGradient * 70.0);
		for(i = 0; i < barWidth; ++i)
			fprintf(histOutput, "*");
		fprintf(histOutput, "\n");
	}
	fprintf(histOutput, "\n");
	*/

	/* Nowozin smoothes the histogram, necessary?*/
	// Average the content of the direction bins.
	for (sn = 0 ; sn < 4 ; ++sn) {
		firstE = bins[0];
		last = bins[36 - 1];
		for (sw = 0 ; sw < 36 ; ++sw) {
			double cur = bins[sw];
			double next = (sw == (36 - 1)) ?
					firstE : bins[(sw + 1) % 36];

			bins[sw] = (last + cur + next) / 3.0;
			last = cur;
		}
	}

	/* find bin with maximum peak */
	maxGradient = 0.0;
	maxBin = 0;
	
    generateKeypointSingle_loop_19(&binIndex, bins, &maxGradient, &maxBin);


	/* output histogram  -- only informational
	fprintf(histOutput, "Pt. %d %d, Level %d (after smoothing)\n", (int) (point->y*this->imageScale), (int) (point->x*this->imageScale), point->imageLevel);
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		fprintf(histOutput, "%2d. ", binIndex);
		barWidth = (int) (bins[binIndex] / maxGradient * 70.0);
		for(i = 0; i < barWidth; ++i)
			fprintf(histOutput, "*");
		fprintf(histOutput, "\n");
	}
	fprintf(histOutput, "\n");
	*/
	
	/*
	 interpolate to find the real peak high */
	left = (maxBin == 0) ? (36 - 1) : (maxBin - 1);
	right = (maxBin + 1) % 36;
	if(!interpolateOrientation(bins[left], bins[maxBin], bins[right], &maxDegreeCorrection, &maxGradient)) {
		printf("    ***fitting parabola failed(1)\n");
		return list;
	}

	/* find other keypoints */
	
    generateKeypointSingle_loop_20(&binIndex, &maxBin, isKeypoint, bins, &maxGradient, &left, &right);


	
    generateKeypointSingle_loop_21(&binIndex, isKeypoint, &left, &right, bins, &maxDegreeCorrection, &maxGradient, &degree, &oneBinRad, &point, &extraCount, &newKey, &lastAddition, &list);


	if(newKey == NULL) {
		return point;
	}
	else
		return lastAddition;
}

//returns true/false if parabolic fit was possible or not
//Nowozin's implementation
// Fit a parabol to the three points (-1.0 ; left), (0.0 ; middle) and
// (1.0 ; right).
//
// Formulas:
// f(x) = a (x - c)^2 + b
//
// c is the peak offset (where f'(x) is zero), b is the peak value.
//
// In case there is an error false is returned, otherwise a correction
// value between [-1 ; 1] is returned in 'degreeCorrection', where -1
// means the peak is located completely at the left vector, and -0.5 just
// in the middle between left and middle and > 0 to the right side. In
// 'peakValue' the maximum estimated peak value is stored.
int interpolateOrientation(double left, double middle, double right, double *degreeCorrection, double *peakValue) {

	double a, b, c;

	*degreeCorrection = *peakValue = 99000000.0;//good enough for DOUBLE_MAX_VALUE?
	a = ((left + right) - 2.0*middle)/2.0;

	/* not a parabola */
	if(a == 0.0)
		return 0;
	c = ((left - middle)/(2.0*a)) - 0.5;
	b = middle - c*c*a;
	if(c < -0.5 || c > 0.5)
		return 0; //something is wrong?
	*degreeCorrection = c;
	*peakValue = b;

	return 1;
}

//calculates a feature vector for each keypoint
//this currently does not work as it should
void createDescriptors(ScaleSpace *this, KeyPoint *list) {

	double sigma;
	const int descriptorDim = 4;
	const int directionCount = 8;
	int    radius;
	KeyPoint *current;
	double angle;
	int    y,x;
	double yR, xR, dir;
	int    imageX, imageY;
	double magnitudeWeight;
	int    iy, ix, id;
	int    i, j, k;
	double dirSpacing, xySpacing;
	double xDist, yDist, dirDist;
	double binX, binY, binDir;

	sigma = descriptorDim/2.0;
	radius = descriptorDim*2; //really, (width=descriptorDim*4)/2
	dirSpacing = (2.0*M_PI)/directionCount;
	xySpacing = 4.0; //each descriptor bin covers 4x4 pixel area
	
    createDescriptors_loop_22(&current, &list, &angle, &descriptorDim, &directionCount, &y, &radius, &x, &imageX, &imageY, &this, &yR, &xR, &dir, &i, &j, &k, &magnitudeWeight, &sigma, &iy, &ix, &id, &binX, &binY, &binDir, &xDist, &xySpacing, &yDist, &dirDist, &dirSpacing);
 //end keypoint traversal loop
}

float ***allocateFeatureVector(int descriptorDim, int directionCount) {

	float ***matrix;
	int i, j, k;

	matrix = (float ***) malloc(descriptorDim*sizeof(float **));
	if(matrix == NULL) {
		printf("malloc returned null\n");
		exit(1);
	}
	
    allocateFeatureVector_loop_23(&i, &descriptorDim, &matrix);

	
    allocateFeatureVector_loop_24(&i, &descriptorDim, &j, &matrix, &directionCount);


	/* initialize */
	
    allocateFeatureVector_loop_25(&i, &descriptorDim, &j, &k, &directionCount, &matrix);

	return matrix;
}

void capAndNormalizeFV(KeyPoint *this) {

	double norm;
	int    i, j, k;

	/* normalize to unit length */
	norm = 0.0;
	
    capAndNormalizeFV_loop_26(&i, &this, &j, &k, &norm);

	norm = sqrt(norm);
	
    capAndNormalizeFV_loop_27(&i, &this, &j, &k, &norm);


	/* cap */
	
    capAndNormalizeFV_loop_28(&i, &this, &j, &k);


	/* renormalize */
	norm = 0.0;
	
    capAndNormalizeFV_loop_29(&i, &this, &j, &k, &norm);

	norm = sqrt(norm);
	
    capAndNormalizeFV_loop_30(&i, &this, &j, &k, &norm);

}

void printKeyPoint(KeyPoint *this, FILE *out) {

	int i, j, k;
	int count = 0;

	fprintf(out, "%.2f %.2f %.2f %.3f\n", this->finalY, this->finalX, this->scale, this->orientation);

	
    printKeyPoint_loop_31(&i, &this, &j, &k, &out, &count);

	fprintf(out, "\n");
}

void markImage(KeyPoint *list, Image* image) {

	KeyPoint *current;
	double   length;
	double   degree;
	int      endX, endY;
	int      arrowX, arrowY;

	
    markImage_loop_32(&current, &list, &length, &degree, &endX, &endY, &image, &arrowX, &arrowY);

}

void markImageSPoint(KeyPoint *list, Image* image, double scale) {

	KeyPoint *current;

	
    markImageSPoint_loop_33(&current, &list, &image, &scale);

}
