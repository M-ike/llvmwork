#include "image_loop_26.h"
void allocateImage_loop_1(int *i, Image * *new, int *j, int *re_arg_pa1_1, Image * *re_arg_pa2_1)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*new)->pic[(*i)] = (float *) malloc((*new)->width*sizeof(float));
		if((*new)->pic[(*i)] == NULL) {
			for((*j) = 0; (*j) < (*i); ++(*j))
				free((*new)->pic[(*i)]);
			free((*new)->pic);
			free((*new));
			printf("***image.c/allocateImage: malloc() returned NULL");
			{ (*re_arg_pa1_1) = 0; (*re_arg_pa2_1) = NULL; return; }
		}
	}

}
void allocateImage_loop_2(int *i, Image * *new, int *j)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i))
		for((*j) = 0; (*j) < (*new)->width; ++(*j))
			(*new)->pic[(*i)][(*j)] = 0.0;

}
void allocateImageFromDoubles_loop_3(int *i, Image * *new, int *j, double ** *array)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*new)->pic[(*i)][(*j)] = (float) (*array)[(*i)][(*j)];
		}
	}

}
void allocateImageFromInts_loop_4(int *i, Image * *new, int *j, int ** *array, int *maxValue)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*new)->pic[(*i)][(*j)] = (float) (*array)[(*i)][(*j)] / (*maxValue);
		}
	}

}
void freeImage_loop_5(int *i, Image * *image)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i))
		free((*image)->pic[(*i)]);

}
void halveImage_loop_6(int *i, Image * *halved, int *j, Image * *original)
{
for((*i) = 0 ; (*i) < (*halved)->height; ++(*i)) {
		for((*j) = 0 ; (*j) < (*halved)->width; ++(*j)) {
			(*halved)->pic[(*i)][(*j)] = (*original)->pic[2*(*i)][2*(*j)];
		}
	}

}
void doubleImage_loop_7(int *i, Image * *original, int *j, Image * *doubled)
{
for((*i) = 0 ; (*i) < (*original)->height; ++(*i)) {
		for ((*j) = 0 ; (*j) < (*original)->width; ++(*j)) {

			/* last row/column */
			if((*i) == (*original)->height-1 && (*j) == (*original)->width-1) {
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
			}
			else if((*i) == (*original)->height - 1) {
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = 0.5*((*original)->pic[(*i)][(*j)] + (*original)->pic[(*i)][(*j)+1]);
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*doubled)->pic[2*(*i) + 0][2*(*j) + 1];
			}
			else if((*j) == (*original)->width - 1) {
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = 0.5*((*original)->pic[(*i)][(*j)] + (*original)->pic[(*i)+1][(*j)]);
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*doubled)->pic[2*(*i) + 1][2*(*j) + 0];
			}
			else {
				// pixel layout:
				// A B
				// C D

				// A
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				// B
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = ((*original)->pic[(*i)][(*j)] +
								(*original)->pic[(*i)][(*j)+1]) / 2.0f;
				// C
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = ((*original)->pic[(*i)][(*j)] +
								(*original)->pic[(*i)+1][(*j)]) / 2.0f;
				// D
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = ((*original)->pic[(*i)][(*j)] +
								(*original)->pic[(*i)][(*j)+1] +
								(*original)->pic[(*i)+1][(*j)] +
								(*original)->pic[(*i)+1][(*j)+1]) / 4.0f;
			}
		}
	}

}
void doubleCanny_loop_8(int *i, Image * *doubled)
{
for((*i) = 0; (*i) < (*doubled)->height; ++(*i)) {
		(*doubled)->pic[(*i)] = (float *) malloc((*doubled)->width*sizeof(float));
	}

}
void doubleCanny_loop_9(int *i, Image * *original, int *j, Image * *doubled)
{
for((*i) = 0 ; (*i) < (*original)->height; ++(*i)) {
		for ((*j) = 0 ; (*j) < (*original)->width; ++(*j)) {
			(*doubled)->pic[2*(*i)][2*(*j)] = (*original)->pic[(*i)][(*j)];
		}
	}

}
void doubleCanny_loop_10(int *i, Image * *original, int *j, Image * *doubled)
{
for((*i) = 0 ; (*i) < (*original)->height; ++(*i)) {
		for ((*j) = 0 ; (*j) < (*original)->width; ++(*j)) {
			
			/* last row/column */
			if((*i) == (*original)->height-1 && (*j) == (*original)->width-1) {
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
			}
			else if((*i) == (*original)->height - 1) {
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = 0.5*((*original)->pic[(*i)][(*j)] + (*original)->pic[(*i)][(*j)+1]);
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*doubled)->pic[2*(*i) + 0][2*(*j) + 1];
			}
			else if((*j) == (*original)->width - 1) {
				(*doubled)->pic[2*(*i) + 0][2*(*j) + 1] = (*original)->pic[(*i)][(*j)];
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 0] = 0.5*((*original)->pic[(*i)][(*j)] + (*original)->pic[(*i)+1][(*j)]);
				(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = (*doubled)->pic[2*(*i) + 1][2*(*j) + 0];
			}
			else {
				// pixel layout:
				// A B
				// C D

				// B
				if((*doubled)->pic[2*(*i)][2*(*j)] == 1.0 && (*doubled)->pic[2*(*i)][2*((*j)+1)] == 1.0) {
					(*doubled)->pic[2*(*i)][2*(*j) + 1] = 1.0;
				}
				else {
					(*doubled)->pic[2*(*i)][2*(*j) + 1] = 0.0;
				}
				
				// C
				if((*doubled)->pic[2*(*i)][2*(*j)] == 1.0 && (*doubled)->pic[2*((*i)+1)][2*(*j)] == 1.0) {
					(*doubled)->pic[2*(*i) + 1][2*(*j)] = 1.0;
				}
				else {
					(*doubled)->pic[2*(*i) + 1][2*(*j)] = 0.0;
				}

				// D
				if(((*doubled)->pic[2*(*i)][2*((*j)+1)] == 1.0 && (*doubled)->pic[2*((*i)+1)][2*(*j)]) || ((*doubled)->pic[2*(*i)][2*(*j)] == 1.0 && (*doubled)->pic[2*((*i)+1)][2*((*j)+1)] == 1.0)) {
					(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = 1.0;
				}
				else {
					(*doubled)->pic[2*(*i) + 1][2*(*j) + 1] = 0.0;
				}
			}
		}
	}

}
void subtractImages_loop_11(int *i, Image * *one, int *j, Image * *newImage, Image * *two)
{
for((*i) = 0; (*i) < (*one)->height; ++(*i))
		for((*j) = 0; (*j) < (*one)->width; ++(*j))
			(*newImage)->pic[(*i)][(*j)] = (*one)->pic[(*i)][(*j)] - (*two)->pic[(*i)][(*j)];

}
void cloneImage_loop_12(int *i, Image * *newImage, int *j, Image * *original)
{
for((*i) = 0; (*i) < (*newImage)->height; ++(*i))
		for((*j) = 0; (*j) < (*newImage)->width; ++(*j))
			(*newImage)->pic[(*i)][(*j)] = (*original)->pic[(*i)][(*j)];

}
void getDerivativeX_loop_13(int *i, Image * *original, float ** *imageX)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*imageX)[(*i)] = (float *) malloc((*original)->width*sizeof(float));
	}

}
void getDerivativeX_loop_14(int *i, Image * *original, int *j, double *sum1, int *p, int maskX[], float ** *imageX)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			(*sum1) = 0.0;
			if((*j) == 0) {
				(*sum1) = (*original)->pic[(*i)][(*j)+1] - (*original)->pic[(*i)][(*j)];
			}
			else if((*j) == (*original)->width - 1) {
				(*sum1) = (*original)->pic[(*i)][(*j)] - (*original)->pic[(*i)][(*j)-1];
			}
			else {
				for((*p) = 0; (*p) <= 2; ++(*p)) {
					(*sum1) += (*original)->pic[(*i)][(*j)-1+(*p)]*maskX[(*p)];
				}
			}
			(*imageX)[(*i)][(*j)] = (*sum1);
		}
	}

}
void getDerivativeY_loop_15(int *i, Image * *original, float ** *imageY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*imageY)[(*i)] = (float *) malloc((*original)->width*sizeof(float));
	}

}
void getDerivativeY_loop_16(int *j, Image * *original, int *i, double *sum2, int *p, int maskY[], float ** *imageY)
{
for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
		for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
			(*sum2) = 0.0;
			if((*i) == 0) {
				(*sum2) = (*original)->pic[(*i)][(*j)] - (*original)->pic[(*i)+1][(*j)];
			}
			else if((*i) == (*original)->height - 1) {
				(*sum2) = (*original)->pic[(*i)-1][(*j)] - (*original)->pic[(*i)][(*j)];
			}
			else {
				for((*p) = 0; (*p) <= 2; ++(*p)) {
					(*sum2) += (*original)->pic[(*i)-1+(*p)][(*j)]*maskY[(*p)];
				}
			}
			(*imageY)[(*i)][(*j)] = (*sum2);
		}
	}

}
void getDerivative_loop_17(int *i, Image * *original, int *j, Image * *newImage, float ** *imageX, float ** *imageY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			(*newImage)->pic[(*i)][(*j)] = (float) sqrt((*imageX)[(*i)][(*j)]*(*imageX)[(*i)][(*j)] + (*imageY)[(*i)][(*j)]*(*imageY)[(*i)][(*j)]);
		}
	}

}
void getDerivative_loop_18(int *i, Image * *original, float ** *imageX, float ** *imageY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		free((*imageX)[(*i)]);
		free((*imageY)[(*i)]);
	}

}
void isPeak_loop_19(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakR, int *re_arg_pa1_19, int *re_arg_pa2_19)
{
for((*i) = (*y), (*j) = (*x); (*i) < (*image)->height && (*i) > 0 && (*j) < (*image)->width && (*j) > 0; (*i) += (*yIncr), (*j) += (*xIncr)) {
		if((*peakVal) < (*image)->pic[(*i)][(*j)])
			{ (*re_arg_pa1_19) = 0; (*re_arg_pa2_19) = 0; return; }
		if((*peakVal) > (*image)->pic[(*i)][(*j)]) {
			(*peakR) = 1;
			break;
		}
	}

}
void isPeak_loop_20(int *i, int *y, int *j, int *x, Image * *image, int *yIncr, int *xIncr, float *peakVal, int *peakL, int *re_arg_pa1_20, int *re_arg_pa2_20)
{
for((*i) = (*y), (*j) = (*x); (*i) < (*image)->height && (*i) > 0 && (*j) < (*image)->width && (*j) > 0; (*i) -= (*yIncr), (*j) -= (*xIncr)) {
		if((*peakVal) < (*image)->pic[(*i)][(*j)])
			{ (*re_arg_pa1_20) = 0; (*re_arg_pa2_20) = 0; return; }
		if((*peakVal) > (*image)->pic[(*i)][(*j)]) {
			(*peakL) = 1;
			break;
		}
	}

}
void drawLine_loop_21(int *i, int *x1, int *x2, int *comp, int *y1, int *dy, int *dx, Image * *image)
{
for((*i) = (*x1); (*i) <= (*x2); ++(*i)) {
			(*comp) = (*y1) + ((*i) - (*x1)) * (*dy) / (*dx);
			if((*comp) >= 0 && (*comp) < (*image)->height &&
						(*i) >= 0 && (*i) < (*image)->width)
				(*image)->pic[(*comp)][(*i)] = 1.0f;
		}

}
void drawLine_loop_22(int *i, int *y1, int *y2, int *comp, int *x1, int *dx, int *dy, Image * *image)
{
for((*i) = (*y1); (*i) <= (*y2); ++(*i)) {
			(*comp) = (*x1) + ((*i) - (*y1)) * (*dx) / (*dy);
			if((*i) >= 0 && (*i) < (*image)->height &&
					(*comp) >= 0 && (*comp) < (*image)->width)
				(*image)->pic[(*i)][(*comp)] = 1.0f;
		}

}
void exportCSV_loop_23(int *i, Image * *image, int *j, FILE * *output)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			fprintf((*output), "%d,%d,%.4f\n", (*j), (*i), (*image)->pic[(*i)][(*j)]);
		}
	}

}
void findMin_loop_24(int *i, Image * *image, int *j, float *min)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->pic[(*i)][(*j)] < (*min))
				(*min) = (*image)->pic[(*i)][(*j)];
		}
	}

}
void findMax_loop_25(int *i, Image * *image, int *j, float *max)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			if((*image)->pic[(*i)][(*j)] > (*max))
				(*max) = (*image)->pic[(*i)][(*j)];
		}
	}

}
void convertToRGB_loop_26(int *i, Image * *image, int *j, ImageRGB * *new)
{
for((*i) = 0; (*i) < (*image)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*image)->width; ++(*j)) {
			(*new)->red->pic[(*i)][(*j)] = (*image)->pic[(*i)][(*j)];
			(*new)->green->pic[(*i)][(*j)] = (*image)->pic[(*i)][(*j)];
			(*new)->blue->pic[(*i)][(*j)] = (*image)->pic[(*i)][(*j)];
		}
	}

}
