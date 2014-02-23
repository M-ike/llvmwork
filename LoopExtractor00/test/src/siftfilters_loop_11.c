#include "siftfilters_loop_11.h"
void localizeIsWeak_loop_1(int *i, int *rows, Matrix * *adj)
{
for((*i) = 0; (*i) < (*rows); ++(*i)) {
			free((*adj)->values[(*i)]);
		}

}
void getAdjustment_loop_2(int *i, gsl_vector * *result, double *sum, gsl_vector * *d)
{
for ((*i) = 0; (*i) < (*result)->size; ++(*i))
		(*sum) += gsl_vector_get((*result), (*i)) * gsl_vector_get((*d), (*i));

}
void getAdjustment_loop_3(int *i, Matrix * *matrixCopy)
{
for((*i) = 0; (*i) < (*matrixCopy)->rows; ++(*i)) {
		(*matrixCopy)->values[(*i)] = (double *) malloc((*matrixCopy)->columns*sizeof(double));
	}

}
void isFingerTip_loop_4(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle)
{
for((*i) = 1; (*i) < (*MAX_SEARCH); ++(*i)) {
		if((*point)->y + (*i) + 1 >= (*this)->imgHeight ||
		   (*point)->x < 1 ||
		   (*point)->x + 1 >= (*this)->imgWidth)
			continue;
		(*val) = (*diffImage)->pic[(*point)->y + (*i)][(*point)->x];
		//printf("  checking val=%f\n", (*val));
		if((*val) > (*PEAK_DEF) && (*val) > (*diffImage)->pic[(*point)->y + (*i) - 1][(*point)->x] && (*val) > (*diffImage)->pic[(*point)->y + (*i) + 1][(*point)->x]) {
			edgeX[(*foundEdges)] = (*point)->x;
			edgeY[(*foundEdges)] = (*point)->y + (*i);
			++(*foundEdges);
			
			(*angle) = atan2((*diffImage)->pic[(*point)->y + (*i) - 1][(*point)->x] - (*diffImage)->pic[(*point)->y + (*i) + 1][(*point)->x], (*diffImage)->pic[(*point)->y + (*i)][(*point)->x + 1] - (*diffImage)->pic[(*point)->y + (*i)][(*point)->x - 1]);
			//printf("+y angle = %f at y=%d x=%d\n", (*angle), (*point)->y + (*i), (*point)->x);
			//printf("  edge val=%f\n",  (*this)->images[(*point)->imageLevel]->pic[(*point)->y + (*i)][(*point)->x]);
			break;
		}
	}

}
void isFingerTip_loop_5(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle)
{
for((*i) = 1; (*i) < (*MAX_SEARCH); ++(*i)) {
		if((*point)->y - (*i) - 1 < 0 ||
		   (*point)->x < 1 ||
		   (*point)->x + 1 >= (*this)->imgWidth)
			continue;
		(*val) = (*diffImage)->pic[(*point)->y - (*i)][(*point)->x];
		//printf("  checking val=%f\n", (*val));
		if((*val) > (*PEAK_DEF) && (*val) > (*diffImage)->pic[(*point)->y - (*i) - 1][(*point)->x] && (*val) > (*diffImage)->pic[(*point)->y - (*i) + 1][(*point)->x]) {
			edgeX[(*foundEdges)] = (*point)->x;
			edgeY[(*foundEdges)] = (*point)->y - (*i);
			++(*foundEdges);
			
			(*angle) = atan2((*diffImage)->pic[(*point)->y - (*i) - 1][(*point)->x] - (*diffImage)->pic[(*point)->y - (*i) + 1][(*point)->x], (*diffImage)->pic[(*point)->y - (*i)][(*point)->x + 1] - (*diffImage)->pic[(*point)->y - (*i)][(*point)->x - 1]);
			//printf("-y angle = %f at y=%d x=%d\n", (*angle), (*point)->y - (*i), (*point)->x);
			//printf("  edge val=%f\n",  (*this)->images[(*point)->imageLevel]->pic[(*point)->y - (*i)][(*point)->x]);
			break;
		}
	}

}
void isFingerTip_loop_6(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, Image * *diffImage, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle)
{
for((*i) = 1; (*i) < (*MAX_SEARCH); ++(*i)) {
		if((*point)->x - (*i) - 1< 0 ||
		   (*point)->y < 1 ||
		   (*point)->y + 1 >= (*this)->imgHeight)
			continue;
		(*val) = (*diffImage)->pic[(*point)->y][(*point)->x - (*i)];
		//printf("  checking val=%f\n", (*val));
		if((*val) > (*PEAK_DEF) && (*val) > (*diffImage)->pic[(*point)->y][(*point)->x - (*i) - 1] && (*val) > (*diffImage)->pic[(*point)->y][(*point)->x - (*i) + 1]) {
			edgeX[(*foundEdges)] = (*point)->x - (*i);
			edgeY[(*foundEdges)] = (*point)->y;
			++(*foundEdges);
			
			(*angle) = atan2((*diffImage)->pic[(*point)->y - 1][(*point)->x - (*i)] - (*diffImage)->pic[(*point)->y + 1][(*point)->x - (*i)], (*diffImage)->pic[(*point)->y][(*point)->x - (*i) + 1] - (*diffImage)->pic[(*point)->y][(*point)->x - (*i) - 1]);
			//printf("-x angle = %f at y=%d x=%d\n", (*angle), (*point)->y, (*point)->x - (*i));
			//printf("  edge val=%f\n",  (*this)->images[(*point)->imageLevel]->pic[(*point)->y][(*point)->x - (*i)]);
			break;
		}
	}

}
void isFingerTip_loop_7(int *i, const int *MAX_SEARCH, KeyPoint * *point, ScaleSpace * *this, double *val, const float *PEAK_DEF, int edgeX[], int *foundEdges, int edgeY[], double *angle, Image * *diffImage)
{
for((*i) = 1; (*i) < (*MAX_SEARCH); ++(*i)) {
		if((*point)->x + (*i) + 1 >= (*this)->imgWidth ||
		   (*point)->y < 1 ||
		   (*point)->y + 1 >= (*this)->imgHeight)
			continue;
		(*val) = (*this)->images[(*point)->imageLevel]->pic[(*point)->y][(*point)->x + (*i)];
		//printf("  checking val=%f\n", (*val));
		if((*val) > (*PEAK_DEF) && (*val) > (*this)->images[(*point)->imageLevel]->pic[(*point)->y][(*point)->x + (*i) - 1] && (*val) > (*this)->images[(*point)->imageLevel]->pic[(*point)->y][(*point)->x + (*i) + 1]) {
			edgeX[(*foundEdges)] = (*point)->x + (*i);
			edgeY[(*foundEdges)] = (*point)->y;
			++(*foundEdges);
			
			(*angle) = atan2((*diffImage)->pic[(*point)->y - 1][(*point)->x + (*i)] - (*diffImage)->pic[(*point)->y + 1][(*point)->x + (*i)], (*diffImage)->pic[(*point)->y][(*point)->x + (*i) + 1] - (*diffImage)->pic[(*point)->y][(*point)->x + (*i) - 1]);
			//printf("+x angle = %f at y=%d x=%d\n", (*angle), (*point)->y, (*point)->x + (*i));
			//printf("  edge val=%f\n",  (*this)->images[(*point)->imageLevel]->pic[(*point)->y][(*point)->x + (*i)]);
			break;
		}
	}

}
void isFingerTip_loop_8(int *i, int *foundEdges, double distance[], int edgeX[], KeyPoint * *point, int edgeY[])
{
for((*i) = 0; (*i) < (*foundEdges); ++(*i)) {
		distance[(*i)] = (edgeX[(*i)] - (*point)->x)*(edgeX[(*i)] - (*point)->x) + (edgeY[(*i)] - (*point)->y)*(edgeY[(*i)] - (*point)->y);
	}

}
void isFingerTip_loop_9(int *i, int *foundEdges, int *j, double distance[], int *re_arg_pa1_9, int *re_arg_pa2_9)
{
for((*i) = 0; (*i) < (*foundEdges); ++(*i)) {
			for((*j) = 0; (*j) < (*foundEdges); ++(*j)) {
				if(distance[(*i)] > 4*distance[(*j)]) {
					//for(k = 0; k < (*foundEdges); ++k) {
					//	printf("y=%d x=%d val=%f\n", edgeY[k], edgeX[k], this->images[point->imageLevel]->pic[edgeY[k]][edgeX[k]]);
					//}
					//printf("***y1=%d x1=%d y2=%d x2=%d\n", edgeY[(*i)], edgeX[(*i)], edgeY[(*j)], edgeX[(*j)]);
					//printf("***d[%d]=%f 2d[%d]=%f\n", (*i),distance[(*i)],(*j), 2*distance[(*j)]);
					{ (*re_arg_pa1_9) = 0; (*re_arg_pa2_9) = 0; return; }
				}
			}
		}

}
void isFingerTip_loop_10(int *i, int *foundEdges, double distance[])
{
for((*i) = 0; (*i) < (*foundEdges) - 1; ++(*i)) {
			if(distance[(*i)+1] > 1.2*distance[(*i)] ||
			   distance[(*i)+1] < 0.8*distance[(*i)])
			   	break;
		}

}
void isFingerTip_loop_11(int *i, int *foundEdges)
{
for((*i) = 0; (*i) < (*foundEdges); ++(*i)) {
		//printf("y=%d x=%d val=%f dist=%f\n", edgeY[(*i)], edgeX[(*i)], this->images[point->imageLevel]->pic[edgeY[(*i)]][edgeX[(*i)]], distance[(*i)]);
	}

}
