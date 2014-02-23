#include "siftfilters_loop_11.h"
KeyPoint *filterAndLocalizePeaks(ScaleSpace *spaces, ScaleSpace *gaussian, KeyPoint *peaks) {

	KeyPoint *filtered = NULL;
	KeyPoint *counter;
	KeyPoint *temp;
	int keeping;
	float edgeRatio;
	char message[64];

	counter = peaks;
	while(counter != NULL) {
		/* experimental, not part of SIFT
		if(counter->minMax) //throw out maxes (not fingertips)
			keeping = 0;
		else
			keeping = 1;
		
		if(keeping)*/
			keeping = !isTooEdgeLike(spaces->images[counter->imageLevel], counter->x, counter->y, &edgeRatio);
		if(!keeping) {
			sprintf(message, "F edge test - %.2f,", edgeRatio);
			sprintf(counter->description, strcat(counter->description, message));
		}
		/* experimental, not part of SIFT
		if(keeping) {
			keeping = isFingerTip(spaces, gaussian, counter);
		}*/
		//localization, contrast check
		
		if(keeping) {
			sprintf(message, "P edge test - %.2f,", edgeRatio);
			sprintf(counter->description, strcat(counter->description, message));
			keeping = !localizeIsWeak(spaces, counter);
		}
		if(keeping) {
			//isFingerTip(spaces, counter);
			//printHistogram(spaces, counter);
			temp = counter;
			counter = counter->next;
			filtered = linkPoint(filtered, temp);
		} else {
			/*
			temp = counter;
			counter = counter->next;
			free(temp);
			*/
			temp = counter;
			counter = counter->next;
			trash = linkPoint(trash, temp);
		}
	}
	return filtered;
}

// Nowozin's implementation
// Return true if the point is not suitable, either because it lies on a
// border pixel or the Hessian matrix cannot be inverted.
// If false is returned, the pixel is suitable for localization and
// additional localization information has been put into 'point.Local'.
// No more than 'steps' corrections are made.
// returns 1 if weak
// returns 0 if OK
int localizeIsWeak (ScaleSpace *this, KeyPoint *point)
{
	int    needToAdjust = 1;
	int    adjusted = 0;
	int    x, y, i, j;
	Image  *image;
	double dotProduct;
	Matrix *adj;
	double adjS, adjY, adjX;
	double distSq;
	int    rows;
	double oldScale, diff, nextScale, newScale;
	double dValue;
	char   message[128] = {'\0'};

	while (needToAdjust) {
		x = point->x;
		y = point->y;

		//Points we cannot say anything about, as they lie on the border
		//of the scale space
		//after adjusting, point can be moved to a new imageLevel
		if (point->imageLevel <= 0 || point->imageLevel >= (this->imageCount - 1)) {
			sprintf(point->description, strcat(point->description, "bordering imageLevel,"));
			return 1;
		}

		image = this->images[point->imageLevel];
		if (x <= 0 || x >= (image->width - 1) ||
		    y <= 0 || y >= (image->height - 1)) {
		    	sprintf(point->description, strcat(point->description, "bordering position,"));
			return 1;
		}

		adj = getAdjustment(this, point, point->imageLevel, &dotProduct);
		if(adj == NULL) {
			printf("***siftfilters.c/localizeIsWeak(): Linear solve returned NULL\n");
			continue;
		}

		// Get adjustments and check if we require further adjustments due
		// to pixel level moves. If so, turn the adjustments into real
		// changes and continue the loop.
		// Nowozin does not do adjustment in sigma dimension
		adjS = adj->values[0][0];
		adjY = adj->values[1][0];
		adjX = adj->values[2][0];

		/* once the values are copied, we don't need the matrix anymore */
		rows = adj->rows;
		
    localizeIsWeak_loop_1(&i, &rows, &adj);

		free(adj);
		adj = NULL;

		if (fabs(adjX) > 0.5 || fabs(adjY) > 0.5 || fabs(adjS) > 0.5) {
			/* give up if too many adjustments */
			if (adjusted >= MAX_ADJ_STEPS) {
				sprintf(message, "F localization - adjX=%.3f adjY=%.3f adjS=%.3f,", adjX, adjY, adjS);
				sprintf(point->description, strcat(point->description, message));
				return 1;
			}
			++adjusted;

			// Check that just one pixel step is needed, otherwise discard
			// the point
			distSq = adjX * adjX + adjY * adjY + adjS*adjS;
			if (distSq > 3.0) {
				sprintf(message, "F 1 pixel step check - %.2f,", distSq);
				sprintf(point->description, strcat(point->description, message));
				return 1;
			}

			/* Nowozin rounds by adding 0.5 to adjustments, but what if
			   adjustment is negative?
			*/
			point->x = (int) (0.5+point->x + adjX);
			point->y = (int) (0.5+point->y + adjY);
			oldScale = point->relativeScale;
			newScale = point->relativeScale + adjS;
			//point->relativeScale = newScale;

			/* determine new imageLevel */
			diff = newScale - oldScale;
			if(diff < 0) {
				nextScale = this->relativeScaleTable[point->imageLevel - 1];
				if((newScale - nextScale) < (oldScale - newScale)) {
					point->imageLevel -= 1;
					point->relativeScale = this->relativeScaleTable[point->imageLevel];
				}
			}
			else {
				nextScale = this->relativeScaleTable[point->imageLevel + 1];
				if((nextScale - newScale) < (newScale - oldScale)) {
					point->imageLevel += 1;
					point->relativeScale = this->relativeScaleTable[point->imageLevel];
				}
			}
			continue;
		}

		sprintf(message, "P localization - %d adj steps - adjX=%.3f adjY=%.3f adjS=%.3f,", adjusted, adjX, adjY, adjS);
		sprintf(point->description, strcat(point->description, message));

		// Check if we already have a keypoint within this octave for this
		// pixel position in order to avoid dupes. (Maybe we can move this
		// check earlier after any adjustment, so we catch dupes earlier).
		// If its not in there, mark it for later searches.
		//
		// dupe = duplicate?
		if (this->processedLocalization[point->y][point->x] != 0) {
			sprintf(point->description, strcat(point->description, "duplicate,"));
			return 1;
		}
		this->processedLocalization[point->y][point->x] = 1;

		/* check for low contrast, pg11 */
		dValue = image->pic[point->y][point->x] + 0.5 * dotProduct;
		if(dValue >= 0 && dValue < CONTRAST_THRESH) {
			sprintf(message, "F contrast test - %f,", dValue);
			sprintf(point->description, strcat(point->description, message));
			return 1;
		}
		if(dValue < 0 && -1*dValue < CONTRAST_THRESH) {
			sprintf(message, "F contrast test - %f,", dValue);
			sprintf(point->description, strcat(point->description, message));
			return 1;
		}

		/* check for hand contrast, not in SIFT
		if(dValue >= 0 && (dValue < 0.059 || dValue > 0.10)) { //betw. fingers
			sprintf(message, "F hand contrast - %f,", dValue);
			sprintf(point->description, strcat(point->description, message));
			return 1;
		}
		if(dValue < 0 && (dValue < -0.10 || dValue > -0.059)) { //fingertips
			sprintf(message, "F hand contrast - %f,", dValue);
			sprintf(point->description, strcat(point->description, message));
			return 1;
		}
		*/

		sprintf(message, "P contrast test - %f,", dValue);
		sprintf(point->description, strcat(point->description, message));

		// Save final sub-pixel adjustments.
		point->finalX = point->x + adjX;
		point->finalY = point->y + adjY;
		point->scale = point->relativeScale + adjS;

		needToAdjust = 0;
	}
	return 0;
}

// Return adjustment (scale, y, x) on success,
// solves Hx = b for x, where b = -d
// return null on failure
// uses GSL library for solving
Matrix *getAdjustment(ScaleSpace *this, KeyPoint *point, int level, double *dotP) {

	Image  *below;
	Image  *current;
	Image  *above;
	int    x, y;
	gsl_matrix *H;
	gsl_vector *b, *result, *d;
	gsl_permutation *p;
	int i;
	double tempValue;
	int s;
	double sum;
	Matrix *matrixCopy;

	*dotP = 0.0;
	x = point->x;
	y = point->y;
	if(point->imageLevel <= 0 || point->imageLevel >= (this->imageCount - 1))
		return NULL;

	below = this->images[point->imageLevel - 1];
	current = this->images[point->imageLevel];
	above = this->images[point->imageLevel + 1];

	H = gsl_matrix_alloc(3, 3);

	tempValue = below->pic[y][x] - 2*current->pic[y][x] + above->pic[y][x];
	gsl_matrix_set(H, 0, 0, tempValue);
	tempValue = 0.25*(above->pic[y+1][x] - above->pic[y-1][x] - (below->pic[y+1][x] - below->pic[y-1][x]));
	gsl_matrix_set(H, 0, 1, tempValue);
	gsl_matrix_set(H, 1, 0, tempValue);
	tempValue = 0.25*(above->pic[y][x+1] - above->pic[y][x-1] - (below->pic[y][x+1] - below->pic[y][x-1]));
	gsl_matrix_set(H, 0, 2, tempValue);
	gsl_matrix_set(H, 2, 0, tempValue);
	tempValue = current->pic[y-1][x] - 2*current->pic[y][x] + current->pic[y+1][x];
	gsl_matrix_set(H, 1, 1, tempValue);
	tempValue = 0.25*(current->pic[y+1][x+1] - current->pic[y+1][x-1] - (current->pic[y-1][x+1] - current->pic[y-1][x-1]));
	gsl_matrix_set(H, 1, 2, tempValue);
	gsl_matrix_set(H, 2, 1, tempValue);
	tempValue = current->pic[y][x-1] - 2*current->pic[y][x] + current->pic[y][x+1];
	gsl_matrix_set(H, 2, 2, tempValue);

	d = gsl_vector_alloc(3);

	tempValue = 0.5*(above->pic[y][x] - below->pic[y][x]);
	gsl_vector_set(d, 0, tempValue);
	tempValue = 0.5*(current->pic[y+1][x] - current->pic[y-1][x]);
	gsl_vector_set(d, 1, tempValue);
	tempValue = 0.5*(current->pic[y][x+1] - current->pic[y][x-1]);
	gsl_vector_set(d, 2, tempValue);

	b = gsl_vector_alloc(3);

	gsl_vector_set(b, 0, -1*gsl_vector_get(d, 0));
	gsl_vector_set(b, 1, -1*gsl_vector_get(d, 1));
	gsl_vector_set(b, 2, -1*gsl_vector_get(d, 2));

	result = gsl_vector_alloc(3);

	p = gsl_permutation_alloc(3);

	gsl_linalg_LU_decomp (H, p, &s);

  	gsl_linalg_LU_solve (H, p, b, result);

	/* dot product */
	sum = 0.0;
	
    getAdjustment_loop_2(&i, &result, &sum, &d);

	*dotP = sum;

	matrixCopy = (Matrix *) malloc(sizeof(Matrix));
	matrixCopy->rows = 3;
	matrixCopy->columns = 1;
	matrixCopy->values = (double **) malloc(matrixCopy->rows*sizeof(double *));
	
    getAdjustment_loop_3(&i, &matrixCopy);

	matrixCopy->values[0][0] = gsl_vector_get(result, 0);
	matrixCopy->values[1][0] = gsl_vector_get(result, 1);
	matrixCopy->values[2][0] = gsl_vector_get(result, 2);

	gsl_matrix_free(H);
	gsl_vector_free(d);
	gsl_vector_free(b);
	gsl_vector_free(result);
	gsl_permutation_free(p);

	return matrixCopy;
}

//returns 1 if the pixel appears to be an edge (discard the point), 0 otherwise (keep it)
//Nowozin's math
int isTooEdgeLike(Image *image, int x, int y, float *edgeRatio) {

	float D_xx, D_yy, D_xy;
	float traceHsquared;
	float determinantH;
	float r1squared;

	/* Calculate the Hessian H elements [ D_xx, D_xy ; D_xy , D_yy ] */
	D_xx = image->pic[y][x+1] + image->pic[y][x-1] - 2.0 * image->pic[y][x];
	D_yy = image->pic[y+1][x] + image->pic[y-1][x] - 2.0 * image->pic[y][x];
	D_xy = 0.25 * ((image->pic[y+1][x+1] - image->pic[y-1][x+1]) -
				(image->pic[y+1][x - 1] - image->pic[y-1][x-1]));

	/* page 12 in Lowe's paper */
	traceHsquared = D_xx + D_yy;
	traceHsquared *= traceHsquared;
	determinantH = (D_xx * D_yy) - (D_xy * D_xy);

	/* if determinant is negative (unlikely), the point is not an extremum */
	if(determinantH < 0.0) {
		return 1;
	}

	/* threshold check */
	r1squared = (EDGE_RATIO + 1.0);
	r1squared *= r1squared;
	*edgeRatio = traceHsquared / determinantH;
	if ((traceHsquared / determinantH) < (r1squared / EDGE_RATIO))
		return 0;
	else
		return 1;
}

//is this keypoint a finger tip?, not in SIFT, my own experiment
int isFingerTip(ScaleSpace *this, ScaleSpace *gaussian, KeyPoint *point) {

	const int MAX_SEARCH = 50;
	int    i,j, k;
	double val, slope, y, x;
	int    edgeX[4], edgeY[4];
	int    foundEdges = 0;
	double distance[4];
	const float PEAK_DEF = 0.008; //0.008
	double angle, yDiff, xDiff;
	Image  *diffImage, *gaussImage;

	diffImage = this->images[point->imageLevel];
	gaussImage = gaussian->images[point->imageLevel];
	//printf("min=%f max=%f\n", this->images[point->imageLevel]->min, this->images[point->imageLevel]->max);
	val = diffImage->pic[point->y][point->x];
	//printf("  starting at val=%f y=%d x=%d\n", val, point->y, point->x);
	if(val > 0) //only accept mins
		return 0;

	
    isFingerTip_loop_4(&i, &MAX_SEARCH, &point, &this, &val, &diffImage, &PEAK_DEF, edgeX, &foundEdges, edgeY, &angle);


	
    isFingerTip_loop_5(&i, &MAX_SEARCH, &point, &this, &val, &diffImage, &PEAK_DEF, edgeX, &foundEdges, edgeY, &angle);


	if(foundEdges <= 0) {
		return 0; //no edge here
	}

	
    isFingerTip_loop_6(&i, &MAX_SEARCH, &point, &this, &val, &diffImage, &PEAK_DEF, edgeX, &foundEdges, edgeY, &angle);


	
    isFingerTip_loop_7(&i, &MAX_SEARCH, &point, &this, &val, &PEAK_DEF, edgeX, &foundEdges, edgeY, &angle, &diffImage);


	//mark points
	/*
	this->images[point->imageLevel]->pic[point->y][point->x] = 1.0;
	this->images[point->imageLevel]->pic[point->y + 1][point->x] = 1.0;
	this->images[point->imageLevel]->pic[point->y - 1][point->x] = 1.0;
	this->images[point->imageLevel]->pic[point->y][point->x + 1] = 1.0;
	this->images[point->imageLevel]->pic[point->y][point->x - 1] = 1.0;

	for(i = 0; i < foundEdges; ++i) {
		this->images[point->imageLevel]->pic[edgeY[i]][edgeX[i]] = 1.0;
	}
	*/

	if(foundEdges <= 2){
		return 0;
	}

	//compute distances sq.
	
    isFingerTip_loop_8(&i, &foundEdges, distance, edgeX, &point, edgeY);


	//also look at peak values, should be similar
	if(foundEdges == 3) {
			{ int re_arg_pa1_9 = -1; int re_arg_pa2_9;
    isFingerTip_loop_9(&i, &foundEdges, &j, distance, &re_arg_pa1_9, &re_arg_pa2_9);
	if(re_arg_pa1_9 != -1) return re_arg_pa2_9; }

	}
	else { //foundEdges == 4
		mergesort(distance, 0, 3);

		//check distances are not all the same (circle)
		
    isFingerTip_loop_10(&i, &foundEdges, distance);

		if(i == foundEdges - 1) {
			printf("***ALL SIMILAR\n");
			return 0;
		}

		//take the first 3 into account
		for(i = 0; i < 3; ++i) {
			for(j = 0; j < 3; ++j) {
				if(distance[i] > 4*distance[j])
					return 0;
			}
		}
	}

	printf("  found edges for y=%d x=%d val=%f:\n", point->y, point->x, this->images[point->imageLevel]->pic[point->y][point->x]);
	
    isFingerTip_loop_11(&i, &foundEdges);


	return 1;
}
