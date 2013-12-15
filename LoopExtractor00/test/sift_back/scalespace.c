/*
*  Jan Prokaj
*  March 5, 2005
*/
#include "scalespace.h"


//Builds the scale space by incrementally convolving until reaching 2*startSigma
//If we want IMAGES_PER_OCTAVE number of usable images, we need to produce + 2
//initial images (or INTERVALS_PER_OCTAVE + 3 as Lowe says). So k is equal
//to 2^(1/s)
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
	for(i = 0; i < space->imageCount; ++i) {
		space->relativeScaleTable[i] = sigma;
		sigma *= k;
	}

	for(i = 0; i < space->imageCount; ++i) {
		if(i == 0)
			sourceBlur = imageSigma;
		else
			sourceBlur = space->relativeScaleTable[i-1];
		targetBlur = space->relativeScaleTable[i];
		if(i == 0 && sourceBlur == targetBlur) {
			space->images[i] = original;
			//printf("      convolution not necessary\n");
		}
		else {
			sigma = sqrt((targetBlur*targetBlur) - (sourceBlur*sourceBlur));
			//printf("      convolution sigma=%f (%f)\n", sigma, sigma*imgScale);
			if(i == 0)
				space->images[i] = gaussianConvolution2DFast(original, sigma);
			else
				space->images[i] = gaussianConvolution2DFast(space->images[i-1], sigma);
		}
	}
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
	for(i = 0; i < space->imageCount; ++i) {
		space->images[i] = subtractImages(original->images[i+1], original->images[i]);
	}

	space->processedLocalization = (int **) malloc(space->imgHeight*sizeof(int *));
	for(i = 0; i < space->imgHeight; ++i) {
		space->processedLocalization[i] = (int *) malloc(space->imgWidth*sizeof(int));
	}
	for(i = 0; i < space->imgHeight; ++i) {
		for(j = 0; j < space->imgWidth; ++j) {
			space->processedLocalization[i][j] = 0;
		}
	}
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
	for(i = 0; i < space->imageCount; ++i) {
		space->images[i] = getDerivative(original->images[i]);
	}

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
	for(i = 0; i < space->imageCount; ++i) {
		space->images[i] = getCannyEdges(derivative->images[i], 1.0, 0.20, gaussian->images[i]);
	}

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
	for(i = 1; i < current->height - 1; ++i) {
		for(j = 1; j < current->width - 1; ++j) {
			min = current->pic[i][j];
			isMin = 1;
			for(p = -1; p <= 1 && isMin; ++p) {
				for(q = -1; q <= 1 && isMin; ++q) {
					if(p != 0 || q != 0) {
						if(current->pic[i+p][j+q] <= min) {
							isMin = 0;
							break;
						}
					}
					if(isMin && above->pic[i+p][j+q] <= min) {
						isMin = 0;
						break;
					}
					if(isMin && below->pic[i+p][j+q] <= min) {
						isMin = 0;
						break;
					}
				}
			}
			if(isMin) {
				/* add new peak */
				temp = (KeyPoint *) malloc(sizeof(KeyPoint));
				temp->minMax = 0; //min
				temp->x = j;
				temp->y = i;
				temp->finalX = j;
				temp->finalY = i;
				temp->imageLevel = DoGlevel;
				temp->imgScale = this->imageScale;
				temp->relativeScale = this->relativeScaleTable[temp->imageLevel];
				temp->scale = temp->relativeScale;
				temp->description[0] = '\0';
				sprintf(temp->description, strcat(temp->description, "min,"));
				peaks = linkPoint(peaks, temp);
			}
			else {
				/* check if maximum */
				max = current->pic[i][j];
				isMax = 1;
				for(p = -1; p <= 1 && isMax; ++p) {
					for(q = -1; q <= 1 && isMax; ++q) {
						if(p != 0 || q != 0) {
							if(current->pic[i+p][j+q] >= max) {
								isMax = 0;
								break;
							}
						}
						if(isMax && above->pic[i+p][j+q] >= max) {
							isMax = 0;
							break;
						}
						if(isMax && below->pic[i+p][j+q] >= max) {
							isMax = 0;
							break;
						}
					}
				}
				if(isMax) {
					/* add new peak */
					temp = (KeyPoint *) malloc(sizeof(KeyPoint));
					temp->minMax = 1;
					temp->x = j;
					temp->y = i;
					temp->finalX = j;
					temp->finalY = i;
					temp->imageLevel = DoGlevel;
					temp->imgScale = this->imageScale;
					temp->relativeScale = this->relativeScaleTable[temp->imageLevel];
					temp->scale = temp->relativeScale;
					temp->description[0] = '\0';
					sprintf(temp->description, strcat(temp->description, "max,"));
					peaks = linkPoint(peaks, temp);
				}
			}
		}
	}
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
	for(y = yMin; y < yMax; ++y) {
		for(x = xMin; x < xMax; ++x) {

			/* only accept points in circle */
			relativeX = x - point->x;
			relativeY = y - point->y;
			if((relativeX*relativeX) + (relativeY*relativeY) <= (radius*radius)) {

				/* find closest bin */
				yDiff = this->images[point->imageLevel]->pic[y-1][x] - this->images[point->imageLevel]->pic[y+1][x];
				xDiff = this->images[point->imageLevel]->pic[y][x+1] - this->images[point->imageLevel]->pic[y][x-1];
				direction = atan2(yDiff, xDiff);

				binIndex = (int) ((direction + M_PI)/oneBinRad);
				if(binIndex == 36)
					binIndex = 0;

				/* put into bin */
				magnitude = sqrt(yDiff*yDiff + xDiff*xDiff);
				bins[binIndex] += magnitude*exp(-1*((relativeX*relativeX + relativeY*relativeY)/(2.0*sigma*sigma)));
			}
		}
	}

	/* find maximum peak for histogram output */
	maxGradient = 0.0;
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		//printf("%d=%f\n", binIndex, bins[binIndex]);
		if(bins[binIndex] > maxGradient) {
			maxGradient = bins[binIndex];
		}
	}

	/* output histogram */
	fprintf(histOutput2, "Pt. %d %d, Level %d\n", (int) (point->y), (int) (point->x), point->imageLevel);
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		fprintf(histOutput2, "%2d. ", binIndex);
		barWidth = (int) (bins[binIndex] / maxGradient * 70.0);
		for(i = 0; i < barWidth; ++i)
			fprintf(histOutput2, "*");
		fprintf(histOutput2, "\n");
	}
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
	for(i = 1; i < height - 1; ++i) {
		for(j = 1; j < width - 1; ++j) {
			for(k = 1; k < imageLevels; ++k) {
				xDiff = this->images[k]->pic[i][j+1] - this->images[k]->pic[i][j-1];
				yDiff = this->images[k]->pic[i-1][j] - this->images[k]->pic[i+1][j]; //because i-1 in array is actually y+1
				this->magnitudes[i][j][k] = sqrt(xDiff*xDiff + yDiff*yDiff);

				this->directions[i][j][k] = atan2(yDiff, xDiff);
			}
		}
	}
}

double ***allocate3D(int height, int width, int depth) {

	int i, j, k;
	double ***matrix;

	matrix = (double ***) malloc(height*sizeof(double **));
	if(matrix == NULL) {
		printf("malloc returned null\n");
		exit(1);
	}
	for(i = 0; i < height; ++i) {
		matrix[i] = (double **) malloc(width*sizeof(double *));
		if(matrix[i] == NULL) {
			printf("malloc returned null\n");
			exit(1);
		}
	}
	for(i = 0; i < height; ++i) {
		for(j = 0; j < width; ++j) {
			matrix[i][j] = (double *) malloc(depth*sizeof(double));
			if(matrix[i][j] == NULL) {
				printf("malloc returned null\n");
				exit(1);
			}
		}
	}
	return matrix;
}

void deallocate3D(double ***matrix, int height, int width) {

	int i, j;

	for(i = 0; i < height; ++i) {
		for(j = 0; j < width; ++j) {
			free(matrix[i][j]);
		}
		free(matrix[i]);
	}
	free(matrix);
}

KeyPoint *generateKeypoints(ScaleSpace *this, KeyPoint *peaks) {

	KeyPoint *current;
	int      multipleKeypoints = 0;

	for(current = peaks; current != NULL; current = current->next) {
		/* create a keypoint */
		current = generateKeypointSingle(this, peaks, current, &multipleKeypoints);
	}
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
	for(y = yMin; y < yMax; ++y) {
		for(x = xMin; x < xMax; ++x) {

			/* only accept points in circle */
			relativeX = x - point->x;
			relativeY = y - point->y;
			if((relativeX*relativeX) + (relativeY*relativeY) <= (radius*radius)) {

				/* find closest bin */
				int binIndex = (int) ((this->directions[y][x][point->imageLevel] + M_PI)/oneBinRad);
				if(binIndex == 36)
					binIndex = 0;

				/* put into bin */
				bins[binIndex] += this->magnitudes[y][x][point->imageLevel]*exp(-1*((relativeX*relativeX + relativeY*relativeY)/(2.0*sigma*sigma)));
			}
		}
	}

	/* find maximum peak for histogram output */
	maxGradient = 0.0;
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		if(bins[binIndex] > maxGradient) {
			maxGradient = bins[binIndex];
		}
	}

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
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		if(bins[binIndex] > maxGradient) {
			maxGradient = bins[binIndex];
			maxBin = binIndex;
		}
	}

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
	for(binIndex = 0; binIndex < 36; ++binIndex) {
		if(binIndex == maxBin) {
			isKeypoint[binIndex] = 2;
			continue;
		}
		if(bins[binIndex] < (PEAK_PERCENT*maxGradient))
			continue;

		/* checks if local peak */
		left = (binIndex == 0) ? (36 - 1) : (binIndex - 1);
		right = (binIndex + 1) % 36;
		if(bins[binIndex] <= bins[left] || bins[binIndex] <= bins[right])
			continue;

		isKeypoint[binIndex] = 1;
	}

	for(binIndex = 0; binIndex < 36; ++binIndex) {
		if(!isKeypoint[binIndex])
			continue;

		/* calculate keypoint orientation */
		left = (binIndex == 0) ? (36 - 1) : (binIndex - 1);
		right = (binIndex + 1) % 36;
		if(!interpolateOrientation(bins[left], bins[binIndex], bins[right], &maxDegreeCorrection, &maxGradient)) {
			printf("    ***fitting parabola failed(2)\n");
			continue;
		}
		degree = (binIndex + maxDegreeCorrection) * oneBinRad - M_PI;
		if(degree < -M_PI)
			degree += 2*M_PI;
		else if(degree > M_PI)
			degree -= 2*M_PI;

		if(isKeypoint[binIndex] == 2) {
			point->orientation =  degree;
		}
		else if(isKeypoint[binIndex] == 1) {
			/* add keypoint to list */
			(*extraCount)++;
			newKey = (KeyPoint *) malloc(sizeof(KeyPoint));
			newKey->finalX = point->finalX;
			newKey->finalY = point->finalY;
			newKey->imageLevel = point->imageLevel;
			newKey->imgScale = point->imgScale;
			newKey->scale = point->scale;
			newKey->orientation = degree;

			if(lastAddition == NULL)
				lastAddition = newKey;

			//special addition to list (in the middle)
			newKey->next = point->next;
			newKey->listRear = point->listRear;
			point->next = newKey;
			list->count = list->count + 1;
			if(list->listRear == point) {
				list->listRear = newKey;
			}
		}
	}

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
	for(current = list; current != NULL; current = current->next) {

		/* for rotating coordinates -- subtracting keypoint orientation */
		angle = -current->orientation;

		/* allocate descriptor */
		current->xDim = descriptorDim;
		current->yDim = descriptorDim;
		current->orientationDim = directionCount;
		current->featureVector = allocateFeatureVector(descriptorDim, directionCount);

		/* fill the descriptor */
		for(y = -radius; y < radius; ++y) {
			for(x = -radius; x < radius; ++x) {
				/* get position */
				imageX = (int) (x + current->finalX);
				imageY = (int) (y + current->finalY);

				/* skip points on border */
				if(imageX < 1 || imageX >= (this->imgWidth - 1) || imageY < 1 || imageY >= (this->imgHeight - 1)) {
					continue;
				}

				/* rotate position by keypoint orientation */
				yR = sin(angle)*x + cos(angle)*y;
				xR = cos(angle)*x - sin(angle)*y;

				/* rotate the gradient direction by keypoint orientation */
				dir = this->directions[imageY][imageX][current->imageLevel] - current->orientation;
				if(dir < -M_PI)
					dir += 2*M_PI;
				else if(dir > M_PI)
					dir -= 2*M_PI;

				/* determine location in descriptor assuming 4x4x8*/
				i = (int) (((yR + 8.0)/16.0)*4.0);
				if(i == 4)
					i = 0;
				j = (int) (((xR + 8.0)/16.0)*4.0);
				if(j == 4)
					j = 0;
				k = (int) (((dir+M_PI)/(2.0*M_PI))*directionCount);
				if(k == directionCount)
					k = 0;

				/* compute weighted magnitude (without normalize factor) */
				magnitudeWeight = exp(-(xR*xR + yR*yR)/(2.0*sigma*sigma))*this->magnitudes[imageY][imageX][current->imageLevel];

				/* calculate feature vector */
				for(iy = -1; iy <= 1; ++iy) {
					for(ix = -1; ix <= 1; ++ix) {
						for(id = -1; id <= 1; ++id) {
							if((i+iy) < 0 ||
							   (i+iy) >= descriptorDim ||
							   (j+ix) < 0 ||
							   (j+ix) >= descriptorDim ||
							   (k + id) < 0 ||
							   (k + id) >= directionCount) {
							   	//printf("out of bounds i=%d j=%d k=%d\n", i, j, k);
							   	continue;
							}
							binX = ((2*(j+ix) + 1)/2.0)*4.0 - 8.0;
							binY = ((2*(i+iy) + 1)/2.0)*4.0 - 8.0;
							binDir = ((2*(k + id) + 1)/2.0)*(M_PI/4.0) - M_PI;
							xDist = (xR - binX)/xySpacing;
							yDist = (yR - binY)/xySpacing;
							dirDist = (dir - binDir)/dirSpacing;

							//printf("   xDist=%f yDist=%f dirDist=%f\n", xDist, yDist, dirDist);
							/* absolute value */
							if(xDist < 0)
								xDist *= -1;
							if(yDist < 0)
								yDist *= -1;
							if(dirDist < 0)
								dirDist *= -1;
							/* farther than one unit */
							if(xDist > 1)
								xDist = 1;
							if(yDist > 1)
								yDist = 1;
							if(dirDist > 1)
								dirDist = 1;
							current->featureVector[i+iy][j+ix][k+id] += (float) (magnitudeWeight*(1.0-xDist)*(1.0-yDist)*(1.0-dirDist));
						}
					}
				} //end descriptor calculation for this sample point
			}
		} //end descriptor calculation for all sample points
		capAndNormalizeFV(current);
	} //end keypoint traversal loop
}

float ***allocateFeatureVector(int descriptorDim, int directionCount) {

	float ***matrix;
	int i, j, k;

	matrix = (float ***) malloc(descriptorDim*sizeof(float **));
	if(matrix == NULL) {
		printf("malloc returned null\n");
		exit(1);
	}
	for(i = 0; i < descriptorDim; ++i) {
		matrix[i] = (float **) malloc(descriptorDim*sizeof(float *));
		if(matrix[i] == NULL) {
			printf("malloc returned null\n");
			exit(1);
		}
	}
	for(i = 0; i < descriptorDim; ++i) {
		for(j = 0; j < descriptorDim; ++j) {
			matrix[i][j] = (float *) malloc(directionCount*sizeof(float));
			if(matrix[i][j] == NULL) {
				printf("malloc returned null\n");
				exit(1);
			}
		}
	}

	/* initialize */
	for(i = 0; i < descriptorDim; ++i) {
		for(j = 0; j < descriptorDim; ++j) {
			for(k = 0; k < directionCount; ++k) {
				matrix[i][j][k] = 0.0f;
			}
		}
	}
	return matrix;
}

void capAndNormalizeFV(KeyPoint *this) {

	double norm;
	int    i, j, k;

	/* normalize to unit length */
	norm = 0.0;
	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				norm += this->featureVector[i][j][k]*this->featureVector[i][j][k];
			}
		}
	}
	norm = sqrt(norm);
	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				this->featureVector[i][j][k] /= norm;
			}
		}
	}

	/* cap */
	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				if(this->featureVector[i][j][k] > CAP)
					this->featureVector[i][j][k] = CAP;
			}
		}
	}

	/* renormalize */
	norm = 0.0;
	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				norm += this->featureVector[i][j][k]*this->featureVector[i][j][k];
			}
		}
	}
	norm = sqrt(norm);
	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				this->featureVector[i][j][k] /= norm;
			}
		}
	}
}

void printKeyPoint(KeyPoint *this, FILE *out) {

	int i, j, k;
	int count = 0;

	fprintf(out, "%.2f %.2f %.2f %.3f\n", this->finalY, this->finalX, this->scale, this->orientation);

	for(i = 0; i < this->yDim; ++i) {
		for(j = 0; j < this->xDim; ++j) {
			for(k = 0; k < this->orientationDim; ++k) {
				fprintf(out, " %d", (int) (255*this->featureVector[i][j][k]));
				//fprintf(out, " %f", this->featureVector[i][j][k]);
				++count;
				if(count >= 8) {
					count = 0;
					fprintf(out, "\n");
				}
			}
		}

	}
	fprintf(out, "\n");
}

void markImage(KeyPoint *list, Image* image) {

	KeyPoint *current;
	double   length;
	double   degree;
	int      endX, endY;
	int      arrowX, arrowY;

	for(current = list; current != NULL; current = current->next) {
		length = current->scale * 5; /* x5 for small scales */
		degree = current->orientation;
		endX = (int) (current->finalX + cos(degree)*length);
		endY = (int) (current->finalY - sin(degree)*length);
		drawLine(image, (int) current->finalY, (int) current->finalX, endY, endX);
		/* left */
		arrowX = (int) (endX + cos(degree + 0.75*M_PI)*(length*0.33));
		arrowY = (int) (endY - sin(degree + 0.75*M_PI)*(length*0.33));
		drawLine(image, endY, endX, arrowY, arrowX);
		/* right */
		arrowX = (int) (endX + cos(degree - 0.75*M_PI)*(length*0.33));
		arrowY = (int) (endY - sin(degree - 0.75*M_PI)*(length*0.33));
		drawLine(image, endY, endX, arrowY, arrowX);
	}
}

void markImageSPoint(KeyPoint *list, Image* image, double scale) {

	KeyPoint *current;

	for(current = list; current != NULL; current = current->next) {
		image->pic[(int) (current->y*scale)][(int) (current->x*scale)] = 1.0;
	}
}
