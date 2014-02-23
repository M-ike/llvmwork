#include "scalespace_loop_33.h"
void buildScaleSpaceOctave_loop_1(int *i, ScaleSpace * *space, double *sigma, double *k)
{
for((*i) = 0; (*i) < (*space)->imageCount; ++(*i)) {
		(*space)->relativeScaleTable[(*i)] = (*sigma);
		(*sigma) *= (*k);
	}

}
void buildScaleSpaceOctave_loop_2(int *i, ScaleSpace * *space, double *sourceBlur, double *imageSigma, double *targetBlur, Image * *original, double *sigma)
{
for((*i) = 0; (*i) < (*space)->imageCount; ++(*i)) {
		if((*i) == 0)
			(*sourceBlur) = (*imageSigma);
		else
			(*sourceBlur) = (*space)->relativeScaleTable[(*i)-1];
		(*targetBlur) = (*space)->relativeScaleTable[(*i)];
		if((*i) == 0 && (*sourceBlur) == (*targetBlur)) {
			(*space)->images[(*i)] = (*original);
			//printf("      convolution not necessary\n");
		}
		else {
			(*sigma) = sqrt(((*targetBlur)*(*targetBlur)) - ((*sourceBlur)*(*sourceBlur)));
			//printf("      convolution sigma=%f (%f)\n", (*sigma), (*sigma)*imgScale);
			if((*i) == 0)
				(*space)->images[(*i)] = gaussianConvolution2DFast((*original), (*sigma));
			else
				(*space)->images[(*i)] = gaussianConvolution2DFast((*space)->images[(*i)-1], (*sigma));
		}
	}

}
void buildDifferences_loop_3(int *i, ScaleSpace * *space, ScaleSpace * *original)
{
for((*i) = 0; (*i) < (*space)->imageCount; ++(*i)) {
		(*space)->images[(*i)] = subtractImages((*original)->images[(*i)+1], (*original)->images[(*i)]);
	}

}
void buildDifferences_loop_4(int *i, ScaleSpace * *space)
{
for((*i) = 0; (*i) < (*space)->imgHeight; ++(*i)) {
		(*space)->processedLocalization[(*i)] = (int *) malloc((*space)->imgWidth*sizeof(int));
	}

}
void buildDifferences_loop_5(int *i, ScaleSpace * *space, int *j)
{
for((*i) = 0; (*i) < (*space)->imgHeight; ++(*i)) {
		for((*j) = 0; (*j) < (*space)->imgWidth; ++(*j)) {
			(*space)->processedLocalization[(*i)][(*j)] = 0;
		}
	}

}
void buildScaleSpaceEdges_loop_6(int *i, ScaleSpace * *space, ScaleSpace * *original)
{
for((*i) = 0; (*i) < (*space)->imageCount; ++(*i)) {
		(*space)->images[(*i)] = getDerivative((*original)->images[(*i)]);
	}

}
void buildScaleSpaceCanny_loop_7(int *i, ScaleSpace * *space, ScaleSpace * *derivative, ScaleSpace * *gaussian)
{
for((*i) = 0; (*i) < (*space)->imageCount; ++(*i)) {
		(*space)->images[(*i)] = getCannyEdges((*derivative)->images[(*i)], 1.0, 0.20, (*gaussian)->images[(*i)]);
	}

}
void findPeaks_loop_8(int *i, Image * *current, int *j, float *min, int *isMin, int *p, int *q, Image * *above, Image * *below, KeyPoint * *temp, int *DoGlevel, ScaleSpace * *this, KeyPoint * *peaks, float *max, int *isMax)
{
for((*i) = 1; (*i) < (*current)->height - 1; ++(*i)) {
		for((*j) = 1; (*j) < (*current)->width - 1; ++(*j)) {
			(*min) = (*current)->pic[(*i)][(*j)];
			(*isMin) = 1;
			for((*p) = -1; (*p) <= 1 && (*isMin); ++(*p)) {
				for((*q) = -1; (*q) <= 1 && (*isMin); ++(*q)) {
					if((*p) != 0 || (*q) != 0) {
						if((*current)->pic[(*i)+(*p)][(*j)+(*q)] <= (*min)) {
							(*isMin) = 0;
							break;
						}
					}
					if((*isMin) && (*above)->pic[(*i)+(*p)][(*j)+(*q)] <= (*min)) {
						(*isMin) = 0;
						break;
					}
					if((*isMin) && (*below)->pic[(*i)+(*p)][(*j)+(*q)] <= (*min)) {
						(*isMin) = 0;
						break;
					}
				}
			}
			if((*isMin)) {
				/* add new peak */
				(*temp) = (KeyPoint *) malloc(sizeof(KeyPoint));
				(*temp)->minMax = 0; //(*min)
				(*temp)->x = (*j);
				(*temp)->y = (*i);
				(*temp)->finalX = (*j);
				(*temp)->finalY = (*i);
				(*temp)->imageLevel = (*DoGlevel);
				(*temp)->imgScale = (*this)->imageScale;
				(*temp)->relativeScale = (*this)->relativeScaleTable[(*temp)->imageLevel];
				(*temp)->scale = (*temp)->relativeScale;
				(*temp)->description[0] = '\0';
				sprintf((*temp)->description, strcat((*temp)->description, "min,"));
				(*peaks) = linkPoint((*peaks), (*temp));
			}
			else {
				/* check if maximum */
				(*max) = (*current)->pic[(*i)][(*j)];
				(*isMax) = 1;
				for((*p) = -1; (*p) <= 1 && (*isMax); ++(*p)) {
					for((*q) = -1; (*q) <= 1 && (*isMax); ++(*q)) {
						if((*p) != 0 || (*q) != 0) {
							if((*current)->pic[(*i)+(*p)][(*j)+(*q)] >= (*max)) {
								(*isMax) = 0;
								break;
							}
						}
						if((*isMax) && (*above)->pic[(*i)+(*p)][(*j)+(*q)] >= (*max)) {
							(*isMax) = 0;
							break;
						}
						if((*isMax) && (*below)->pic[(*i)+(*p)][(*j)+(*q)] >= (*max)) {
							(*isMax) = 0;
							break;
						}
					}
				}
				if((*isMax)) {
					/* add new peak */
					(*temp) = (KeyPoint *) malloc(sizeof(KeyPoint));
					(*temp)->minMax = 1;
					(*temp)->x = (*j);
					(*temp)->y = (*i);
					(*temp)->finalX = (*j);
					(*temp)->finalY = (*i);
					(*temp)->imageLevel = (*DoGlevel);
					(*temp)->imgScale = (*this)->imageScale;
					(*temp)->relativeScale = (*this)->relativeScaleTable[(*temp)->imageLevel];
					(*temp)->scale = (*temp)->relativeScale;
					(*temp)->description[0] = '\0';
					sprintf((*temp)->description, strcat((*temp)->description, "max,"));
					(*peaks) = linkPoint((*peaks), (*temp));
				}
			}
		}
	}

}
void printHistogram_loop_9(int *y, int *yMin, int *yMax, int *x, int *xMin, int *xMax, int *relativeX, KeyPoint * *point, int *relativeY, double *radius, double *yDiff, ScaleSpace * *this, double *xDiff, double *direction, int *binIndex, double *oneBinRad, double *magnitude, double bins[], double *sigma)
{
for((*y) = (*yMin); (*y) < (*yMax); ++(*y)) {
		for((*x) = (*xMin); (*x) < (*xMax); ++(*x)) {

			/* only accept points in circle */
			(*relativeX) = (*x) - (*point)->x;
			(*relativeY) = (*y) - (*point)->y;
			if(((*relativeX)*(*relativeX)) + ((*relativeY)*(*relativeY)) <= ((*radius)*(*radius))) {

				/* find closest bin */
				(*yDiff) = (*this)->images[(*point)->imageLevel]->pic[(*y)-1][(*x)] - (*this)->images[(*point)->imageLevel]->pic[(*y)+1][(*x)];
				(*xDiff) = (*this)->images[(*point)->imageLevel]->pic[(*y)][(*x)+1] - (*this)->images[(*point)->imageLevel]->pic[(*y)][(*x)-1];
				(*direction) = atan2((*yDiff), (*xDiff));

				(*binIndex) = (int) (((*direction) + M_PI)/(*oneBinRad));
				if((*binIndex) == 36)
					(*binIndex) = 0;

				/* put into bin */
				(*magnitude) = sqrt((*yDiff)*(*yDiff) + (*xDiff)*(*xDiff));
				bins[(*binIndex)] += (*magnitude)*exp(-1*(((*relativeX)*(*relativeX) + (*relativeY)*(*relativeY))/(2.0*(*sigma)*(*sigma))));
			}
		}
	}

}
void printHistogram_loop_10(int *binIndex, double bins[], double *maxGradient)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		//printf("%d=%f\n", (*binIndex), bins[(*binIndex)]);
		if(bins[(*binIndex)] > (*maxGradient)) {
			(*maxGradient) = bins[(*binIndex)];
		}
	}

}
void printHistogram_loop_11(int *binIndex, int *barWidth, double bins[], double *maxGradient, int *i)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		fprintf(histOutput2, "%2d. ", (*binIndex));
		(*barWidth) = (int) (bins[(*binIndex)] / (*maxGradient) * 70.0);
		for((*i) = 0; (*i) < (*barWidth); ++(*i))
			fprintf(histOutput2, "*");
		fprintf(histOutput2, "\n");
	}

}
void getMagnitudesAndOrientations_loop_12(int *i, int *height, int *j, int *width, int *k, int *imageLevels, double *xDiff, ScaleSpace * *this, double *yDiff)
{
for((*i) = 1; (*i) < (*height) - 1; ++(*i)) {
		for((*j) = 1; (*j) < (*width) - 1; ++(*j)) {
			for((*k) = 1; (*k) < (*imageLevels); ++(*k)) {
				(*xDiff) = (*this)->images[(*k)]->pic[(*i)][(*j)+1] - (*this)->images[(*k)]->pic[(*i)][(*j)-1];
				(*yDiff) = (*this)->images[(*k)]->pic[(*i)-1][(*j)] - (*this)->images[(*k)]->pic[(*i)+1][(*j)]; //because (*i)-1 in array is actually y+1
				(*this)->magnitudes[(*i)][(*j)][(*k)] = sqrt((*xDiff)*(*xDiff) + (*yDiff)*(*yDiff));

				(*this)->directions[(*i)][(*j)][(*k)] = atan2((*yDiff), (*xDiff));
			}
		}
	}

}
void allocate3D_loop_13(int *i, int *height, double *** *matrix, int *width)
{
for((*i) = 0; (*i) < (*height); ++(*i)) {
		(*matrix)[(*i)] = (double **) malloc((*width)*sizeof(double *));
		if((*matrix)[(*i)] == NULL) {
			printf("malloc returned null\n");
			exit(1);
		}
	}

}
void allocate3D_loop_14(int *i, int *height, int *j, int *width, double *** *matrix, int *depth)
{
for((*i) = 0; (*i) < (*height); ++(*i)) {
		for((*j) = 0; (*j) < (*width); ++(*j)) {
			(*matrix)[(*i)][(*j)] = (double *) malloc((*depth)*sizeof(double));
			if((*matrix)[(*i)][(*j)] == NULL) {
				printf("malloc returned null\n");
				exit(1);
			}
		}
	}

}
void deallocate3D_loop_15(int *i, int *height, int *j, int *width, double *** *matrix)
{
for((*i) = 0; (*i) < (*height); ++(*i)) {
		for((*j) = 0; (*j) < (*width); ++(*j)) {
			free((*matrix)[(*i)][(*j)]);
		}
		free((*matrix)[(*i)]);
	}

}
void generateKeypoints_loop_16(KeyPoint * *current, KeyPoint * *peaks, ScaleSpace * *this, int *multipleKeypoints)
{
for((*current) = (*peaks); (*current) != NULL; (*current) = (*current)->next) {
		/* create a keypoint */
		(*current) = generateKeypointSingle((*this), (*peaks), (*current), &(*multipleKeypoints));
	}

}
void generateKeypointSingle_loop_17(int *y, int *yMin, int *yMax, int *x, int *xMin, int *xMax, int *relativeX, KeyPoint * *point, int *relativeY, double *radius, ScaleSpace * *this, double *oneBinRad, double bins[], double *sigma)
{
for((*y) = (*yMin); (*y) < (*yMax); ++(*y)) {
		for((*x) = (*xMin); (*x) < (*xMax); ++(*x)) {

			/* only accept points in circle */
			(*relativeX) = (*x) - (*point)->x;
			(*relativeY) = (*y) - (*point)->y;
			if(((*relativeX)*(*relativeX)) + ((*relativeY)*(*relativeY)) <= ((*radius)*(*radius))) {

				/* find closest bin */
				int binIndex = (int) (((*this)->directions[(*y)][(*x)][(*point)->imageLevel] + M_PI)/(*oneBinRad));
				if(binIndex == 36)
					binIndex = 0;

				/* put into bin */
				bins[binIndex] += (*this)->magnitudes[(*y)][(*x)][(*point)->imageLevel]*exp(-1*(((*relativeX)*(*relativeX) + (*relativeY)*(*relativeY))/(2.0*(*sigma)*(*sigma))));
			}
		}
	}

}
void generateKeypointSingle_loop_18(int *binIndex, double bins[], double *maxGradient)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		if(bins[(*binIndex)] > (*maxGradient)) {
			(*maxGradient) = bins[(*binIndex)];
		}
	}

}
void generateKeypointSingle_loop_19(int *binIndex, double bins[], double *maxGradient, int *maxBin)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		if(bins[(*binIndex)] > (*maxGradient)) {
			(*maxGradient) = bins[(*binIndex)];
			(*maxBin) = (*binIndex);
		}
	}

}
void generateKeypointSingle_loop_20(int *binIndex, int *maxBin, int isKeypoint[], double bins[], double *maxGradient, int *left, int *right)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		if((*binIndex) == (*maxBin)) {
			isKeypoint[(*binIndex)] = 2;
			continue;
		}
		if(bins[(*binIndex)] < (PEAK_PERCENT*(*maxGradient)))
			continue;

		/* checks if local peak */
		(*left) = ((*binIndex) == 0) ? (36 - 1) : ((*binIndex) - 1);
		(*right) = ((*binIndex) + 1) % 36;
		if(bins[(*binIndex)] <= bins[(*left)] || bins[(*binIndex)] <= bins[(*right)])
			continue;

		isKeypoint[(*binIndex)] = 1;
	}

}
void generateKeypointSingle_loop_21(int *binIndex, int isKeypoint[], int *left, int *right, double bins[], double *maxDegreeCorrection, double *maxGradient, double *degree, double *oneBinRad, KeyPoint * *point, int * *extraCount, KeyPoint * *newKey, KeyPoint * *lastAddition, KeyPoint * *list)
{
for((*binIndex) = 0; (*binIndex) < 36; ++(*binIndex)) {
		if(!isKeypoint[(*binIndex)])
			continue;

		/* calculate keypoint orientation */
		(*left) = ((*binIndex) == 0) ? (36 - 1) : ((*binIndex) - 1);
		(*right) = ((*binIndex) + 1) % 36;
		if(!interpolateOrientation(bins[(*left)], bins[(*binIndex)], bins[(*right)], &(*maxDegreeCorrection), &(*maxGradient))) {
			printf("    ***fitting parabola failed(2)\n");
			continue;
		}
		(*degree) = ((*binIndex) + (*maxDegreeCorrection)) * (*oneBinRad) - M_PI;
		if((*degree) < -M_PI)
			(*degree) += 2*M_PI;
		else if((*degree) > M_PI)
			(*degree) -= 2*M_PI;

		if(isKeypoint[(*binIndex)] == 2) {
			(*point)->orientation =  (*degree);
		}
		else if(isKeypoint[(*binIndex)] == 1) {
			/* add keypoint to (*list) */
			(*(*extraCount))++;
			(*newKey) = (KeyPoint *) malloc(sizeof(KeyPoint));
			(*newKey)->finalX = (*point)->finalX;
			(*newKey)->finalY = (*point)->finalY;
			(*newKey)->imageLevel = (*point)->imageLevel;
			(*newKey)->imgScale = (*point)->imgScale;
			(*newKey)->scale = (*point)->scale;
			(*newKey)->orientation = (*degree);

			if((*lastAddition) == NULL)
				(*lastAddition) = (*newKey);

			//special addition to (*list) (in the middle)
			(*newKey)->next = (*point)->next;
			(*newKey)->listRear = (*point)->listRear;
			(*point)->next = (*newKey);
			(*list)->count = (*list)->count + 1;
			if((*list)->listRear == (*point)) {
				(*list)->listRear = (*newKey);
			}
		}
	}

}
void createDescriptors_loop_22(KeyPoint * *current, KeyPoint * *list, double *angle, const int *descriptorDim, const int *directionCount, int *y, int *radius, int *x, int *imageX, int *imageY, ScaleSpace * *this, double *yR, double *xR, double *dir, int *i, int *j, int *k, double *magnitudeWeight, double *sigma, int *iy, int *ix, int *id, double *binX, double *binY, double *binDir, double *xDist, double *xySpacing, double *yDist, double *dirDist, double *dirSpacing)
{
for((*current) = (*list); (*current) != NULL; (*current) = (*current)->next) {

		/* for rotating coordinates -- subtracting keypoint orientation */
		(*angle) = -(*current)->orientation;

		/* allocate descriptor */
		(*current)->xDim = (*descriptorDim);
		(*current)->yDim = (*descriptorDim);
		(*current)->orientationDim = (*directionCount);
		(*current)->featureVector = allocateFeatureVector((*descriptorDim), (*directionCount));

		/* fill the descriptor */
		for((*y) = -(*radius); (*y) < (*radius); ++(*y)) {
			for((*x) = -(*radius); (*x) < (*radius); ++(*x)) {
				/* get position */
				(*imageX) = (int) ((*x) + (*current)->finalX);
				(*imageY) = (int) ((*y) + (*current)->finalY);

				/* skip points on border */
				if((*imageX) < 1 || (*imageX) >= ((*this)->imgWidth - 1) || (*imageY) < 1 || (*imageY) >= ((*this)->imgHeight - 1)) {
					continue;
				}

				/* rotate position by keypoint orientation */
				(*yR) = sin((*angle))*(*x) + cos((*angle))*(*y);
				(*xR) = cos((*angle))*(*x) - sin((*angle))*(*y);

				/* rotate the gradient direction by keypoint orientation */
				(*dir) = (*this)->directions[(*imageY)][(*imageX)][(*current)->imageLevel] - (*current)->orientation;
				if((*dir) < -M_PI)
					(*dir) += 2*M_PI;
				else if((*dir) > M_PI)
					(*dir) -= 2*M_PI;

				/* determine location in descriptor assuming 4x4x8*/
				(*i) = (int) ((((*yR) + 8.0)/16.0)*4.0);
				if((*i) == 4)
					(*i) = 0;
				(*j) = (int) ((((*xR) + 8.0)/16.0)*4.0);
				if((*j) == 4)
					(*j) = 0;
				(*k) = (int) ((((*dir)+M_PI)/(2.0*M_PI))*(*directionCount));
				if((*k) == (*directionCount))
					(*k) = 0;

				/* compute weighted magnitude (without normalize factor) */
				(*magnitudeWeight) = exp(-((*xR)*(*xR) + (*yR)*(*yR))/(2.0*(*sigma)*(*sigma)))*(*this)->magnitudes[(*imageY)][(*imageX)][(*current)->imageLevel];

				/* calculate feature vector */
				for((*iy) = -1; (*iy) <= 1; ++(*iy)) {
					for((*ix) = -1; (*ix) <= 1; ++(*ix)) {
						for((*id) = -1; (*id) <= 1; ++(*id)) {
							if(((*i)+(*iy)) < 0 ||
							   ((*i)+(*iy)) >= (*descriptorDim) ||
							   ((*j)+(*ix)) < 0 ||
							   ((*j)+(*ix)) >= (*descriptorDim) ||
							   ((*k) + (*id)) < 0 ||
							   ((*k) + (*id)) >= (*directionCount)) {
							   	//printf("out of bounds i=%d j=%d k=%d\n", (*i), (*j), (*k));
							   	continue;
							}
							(*binX) = ((2*((*j)+(*ix)) + 1)/2.0)*4.0 - 8.0;
							(*binY) = ((2*((*i)+(*iy)) + 1)/2.0)*4.0 - 8.0;
							(*binDir) = ((2*((*k) + (*id)) + 1)/2.0)*(M_PI/4.0) - M_PI;
							(*xDist) = ((*xR) - (*binX))/(*xySpacing);
							(*yDist) = ((*yR) - (*binY))/(*xySpacing);
							(*dirDist) = ((*dir) - (*binDir))/(*dirSpacing);

							//printf("   xDist=%f yDist=%f dirDist=%f\n", (*xDist), (*yDist), (*dirDist));
							/* absolute value */
							if((*xDist) < 0)
								(*xDist) *= -1;
							if((*yDist) < 0)
								(*yDist) *= -1;
							if((*dirDist) < 0)
								(*dirDist) *= -1;
							/* farther than one unit */
							if((*xDist) > 1)
								(*xDist) = 1;
							if((*yDist) > 1)
								(*yDist) = 1;
							if((*dirDist) > 1)
								(*dirDist) = 1;
							(*current)->featureVector[(*i)+(*iy)][(*j)+(*ix)][(*k)+(*id)] += (float) ((*magnitudeWeight)*(1.0-(*xDist))*(1.0-(*yDist))*(1.0-(*dirDist)));
						}
					}
				} //end descriptor calculation for (*this) sample point
			}
		} //end descriptor calculation for all sample points
		capAndNormalizeFV((*current));
	}

}
void allocateFeatureVector_loop_23(int *i, int *descriptorDim, float *** *matrix)
{
for((*i) = 0; (*i) < (*descriptorDim); ++(*i)) {
		(*matrix)[(*i)] = (float **) malloc((*descriptorDim)*sizeof(float *));
		if((*matrix)[(*i)] == NULL) {
			printf("malloc returned null\n");
			exit(1);
		}
	}

}
void allocateFeatureVector_loop_24(int *i, int *descriptorDim, int *j, float *** *matrix, int *directionCount)
{
for((*i) = 0; (*i) < (*descriptorDim); ++(*i)) {
		for((*j) = 0; (*j) < (*descriptorDim); ++(*j)) {
			(*matrix)[(*i)][(*j)] = (float *) malloc((*directionCount)*sizeof(float));
			if((*matrix)[(*i)][(*j)] == NULL) {
				printf("malloc returned null\n");
				exit(1);
			}
		}
	}

}
void allocateFeatureVector_loop_25(int *i, int *descriptorDim, int *j, int *k, int *directionCount, float *** *matrix)
{
for((*i) = 0; (*i) < (*descriptorDim); ++(*i)) {
		for((*j) = 0; (*j) < (*descriptorDim); ++(*j)) {
			for((*k) = 0; (*k) < (*directionCount); ++(*k)) {
				(*matrix)[(*i)][(*j)][(*k)] = 0.0f;
			}
		}
	}

}
void capAndNormalizeFV_loop_26(int *i, KeyPoint * *this, int *j, int *k, double *norm)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				(*norm) += (*this)->featureVector[(*i)][(*j)][(*k)]*(*this)->featureVector[(*i)][(*j)][(*k)];
			}
		}
	}

}
void capAndNormalizeFV_loop_27(int *i, KeyPoint * *this, int *j, int *k, double *norm)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				(*this)->featureVector[(*i)][(*j)][(*k)] /= (*norm);
			}
		}
	}

}
void capAndNormalizeFV_loop_28(int *i, KeyPoint * *this, int *j, int *k)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				if((*this)->featureVector[(*i)][(*j)][(*k)] > CAP)
					(*this)->featureVector[(*i)][(*j)][(*k)] = CAP;
			}
		}
	}

}
void capAndNormalizeFV_loop_29(int *i, KeyPoint * *this, int *j, int *k, double *norm)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				(*norm) += (*this)->featureVector[(*i)][(*j)][(*k)]*(*this)->featureVector[(*i)][(*j)][(*k)];
			}
		}
	}

}
void capAndNormalizeFV_loop_30(int *i, KeyPoint * *this, int *j, int *k, double *norm)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				(*this)->featureVector[(*i)][(*j)][(*k)] /= (*norm);
			}
		}
	}

}
void printKeyPoint_loop_31(int *i, KeyPoint * *this, int *j, int *k, FILE * *out, int *count)
{
for((*i) = 0; (*i) < (*this)->yDim; ++(*i)) {
		for((*j) = 0; (*j) < (*this)->xDim; ++(*j)) {
			for((*k) = 0; (*k) < (*this)->orientationDim; ++(*k)) {
				fprintf((*out), " %d", (int) (255*(*this)->featureVector[(*i)][(*j)][(*k)]));
				//fprintf((*out), " %f", (*this)->featureVector[(*i)][(*j)][(*k)]);
				++(*count);
				if((*count) >= 8) {
					(*count) = 0;
					fprintf((*out), "\n");
				}
			}
		}

	}

}
void markImage_loop_32(KeyPoint * *current, KeyPoint * *list, double *length, double *degree, int *endX, int *endY, Image * *image, int *arrowX, int *arrowY)
{
for((*current) = (*list); (*current) != NULL; (*current) = (*current)->next) {
		(*length) = (*current)->scale * 5; /* x5 for small scales */
		(*degree) = (*current)->orientation;
		(*endX) = (int) ((*current)->finalX + cos((*degree))*(*length));
		(*endY) = (int) ((*current)->finalY - sin((*degree))*(*length));
		drawLine((*image), (int) (*current)->finalY, (int) (*current)->finalX, (*endY), (*endX));
		/* left */
		(*arrowX) = (int) ((*endX) + cos((*degree) + 0.75*M_PI)*((*length)*0.33));
		(*arrowY) = (int) ((*endY) - sin((*degree) + 0.75*M_PI)*((*length)*0.33));
		drawLine((*image), (*endY), (*endX), (*arrowY), (*arrowX));
		/* right */
		(*arrowX) = (int) ((*endX) + cos((*degree) - 0.75*M_PI)*((*length)*0.33));
		(*arrowY) = (int) ((*endY) - sin((*degree) - 0.75*M_PI)*((*length)*0.33));
		drawLine((*image), (*endY), (*endX), (*arrowY), (*arrowX));
	}

}
void markImageSPoint_loop_33(KeyPoint * *current, KeyPoint * *list, Image * *image, double *scale)
{
for((*current) = (*list); (*current) != NULL; (*current) = (*current)->next) {
		(*image)->pic[(int) ((*current)->y*(*scale))][(int) ((*current)->x*(*scale))] = 1.0;
	}

}
