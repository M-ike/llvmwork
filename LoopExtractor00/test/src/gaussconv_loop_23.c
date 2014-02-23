#include "gaussconv_loop_23.h"
void gaussianD1Convolution2D_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		(*maskX)[(*i)] = (double *) malloc((*maskSize)*sizeof(double));
		(*maskY)[(*i)] = (double *) malloc((*maskSize)*sizeof(double));
	}

}
void gaussianD1Convolution2D_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*outpicX)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
		(*outpicY)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
	}

}
void gaussianD1Convolution2D_loop_3(int *i, Image * *outImage)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		(*outImage)->pic[(*i)] = (float *) malloc((*outImage)->width*sizeof(float));
	}

}
void gaussianD1Convolution2D_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY)
{
for((*p)=-(*mr);(*p)<=(*mr);(*p)++) {
           for((*q)=-(*mr);(*q)<=(*mr);(*q)++) {
	      (*maskval) = -1*(*normalizeFactor)*((*q)/((*sigma)*(*sigma)))*exp(-1*(((*p)*(*p) + (*q)*(*q))/(2*((*sigma)*(*sigma)))));
              (*maskX)[(*p)+(*centY)][(*q)+(*centX)] = (*maskval);
	      (*maskval) = -1*(*normalizeFactor)*((*p)/((*sigma)*(*sigma)))*exp(-1*(((*p)*(*p) + (*q)*(*q))/(2*((*sigma)*(*sigma)))));
	      (*maskY)[(*p)+(*centY)][(*q)+(*centX)] = (*maskval);
           }
        }

}
void gaussianD1Convolution2D_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY)
{
for((*i)=(*mr); (*i) < (*original)->height - (*mr); (*i)++) {
          for((*j)=(*mr); (*j)< (*original)->width - (*mr); (*j)++) {
             (*sum1) = 0.0;
             (*sum2) = 0.0;
             for ((*p)=-(*mr);(*p)<=(*mr);(*p)++) {
                for ((*q)=-(*mr);(*q)<=(*mr);(*q)++) {
                   (*sum1) += (*original)->pic[(*i)+(*p)][(*j)+(*q)] * (*maskX)[(*p)+(*centY)][(*q)+(*centX)];
                   (*sum2) += (*original)->pic[(*i)+(*p)][(*j)+(*q)] * (*maskY)[(*p)+(*centY)][(*q)+(*centX)];
                }
             }
             (*outpicX)[(*i)][(*j)] = (*sum1);
             (*outpicY)[(*i)][(*j)] = (*sum2);
          }
        }

}
void gaussianD1Convolution2D_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; (*i)++) {
          for((*j) = 0; (*j) < (*original)->width; (*j)++) {
	  	(*outImage)->pic[(*i)][(*j)] = 0.0f;
	  	if((*i) >= (*mr) && (*i) < (*original)->height - (*mr) &&
					(*j) >= (*mr) && (*j) < (*original)->width - (*mr)) {
			(*outImage)->pic[(*i)][(*j)]=(float) sqrt(((*outpicX)[(*i)][(*j)]*(*outpicX)[(*i)][(*j)]) +
							((*outpicY)[(*i)][(*j)]*(*outpicY)[(*i)][(*j)]));
		}
           }
        }

}
void gaussianD1Convolution2D_loop_7(int *i, int *maskSize, double ** *maskX, double ** *maskY)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		free((*maskX)[(*i)]);
		free((*maskY)[(*i)]);
	}

}
void gaussianD1Convolution2D_loop_8(int *i, Image * *original, double ** *outpicX, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		free((*outpicX)[(*i)]);
		free((*outpicY)[(*i)]);
	}

}
void gaussianConvolution2D_loop_9(int *i, int *maskSize, double ** *mask)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		(*mask)[(*i)] = (double *) malloc((*maskSize)*sizeof(double));
	}

}
void gaussianConvolution2D_loop_10(int *i, Image * *original, double ** *outpic)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*outpic)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
	}

}
void gaussianConvolution2D_loop_11(int *i, Image * *outImage)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		(*outImage)->pic[(*i)] = (float *) malloc((*outImage)->width*sizeof(float));
	}

}
void gaussianConvolution2D_loop_12(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, double *sigma, double ** *mask, int *centY, int *centX)
{
for((*p)=-(*mr);(*p)<=(*mr);(*p)++) {
           for((*q)=-(*mr);(*q)<=(*mr);(*q)++) {
	      (*maskval) = (*normalizeFactor)*exp(-1*(((*p)*(*p) + (*q)*(*q))/(2*((*sigma)*(*sigma)))));
              (*mask)[(*p)+(*centY)][(*q)+(*centX)] = (*maskval);
           }
        }

}
void gaussianConvolution2D_loop_13(int *i, int *mr, Image * *original, int *j, double *sum, int *p, int *q, double ** *mask, int *centY, int *centX, double ** *outpic)
{
for((*i)=(*mr); (*i) < (*original)->height - (*mr); (*i)++) {
          for((*j)=(*mr); (*j)< (*original)->width - (*mr); (*j)++) {
             (*sum) = 0.0;
             for ((*p)=-(*mr);(*p)<=(*mr);(*p)++) {
                for ((*q)=-(*mr);(*q)<=(*mr);(*q)++) {
                   (*sum) += (*original)->pic[(*i)+(*p)][(*j)+(*q)] * (*mask)[(*p)+(*centY)][(*q)+(*centX)];
                }
             }
             (*outpic)[(*i)][(*j)] = (*sum);
          }
        }

}
void gaussianConvolution2D_loop_14(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpic)
{
for((*i) = 0; (*i) < (*original)->height; (*i)++) {
          for((*j) = 0; (*j) < (*original)->width; (*j)++) {
	  	(*outImage)->pic[(*i)][(*j)] = 0.0f;
	  	if((*i) >= (*mr) && (*i) < (*original)->height - (*mr) &&
					(*j) >= (*mr) && (*j) < (*original)->width - (*mr)) {
			(*outImage)->pic[(*i)][(*j)]=(float) (*outpic)[(*i)][(*j)];
		}
           }
        }

}
void gaussianConvolution2D_loop_15(int *i, int *maskSize, double ** *mask)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		free((*mask)[(*i)]);
	}

}
void gaussianConvolution2D_loop_16(int *i, Image * *original, double ** *outpic)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		free((*outpic)[(*i)]);
	}

}
void gaussianConvolution2DFast_loop_17(int *i, Image * *original, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*outpicY)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
	}

}
void gaussianConvolution2DFast_loop_18(int *i, Image * *outImage)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		(*outImage)->pic[(*i)] = (float *) malloc((*outImage)->width*sizeof(float));
	}

}
void gaussianConvolution2DFast_loop_19(int *i, Image * *original, int *j, double ** *outpicY, Image * *outImage)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			(*outpicY)[(*i)][(*j)] = 0.0;
			(*outImage)->pic[(*i)][(*j)] = 0.0f;
		}
	}

}
void gaussianConvolution2DFast_loop_20(int *i, int *maskSize, int *p, int *maskMiddle, double * *mask, double *normalizeFactor, double *sigma)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		(*p) = (*i) - (*maskMiddle);
		(*mask)[(*i)] = (*normalizeFactor)*exp(-1*((*p)*(*p))/(2.0*(*sigma)*(*sigma)));
	}

}
void gaussianConvolution2DFast_loop_21(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->width; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->height; ++(*j)) {
			(*sum) = 0.0;
			(*isOut) = 0;
			(*outBound) = 0.0;
			for((*p) = 0; (*p) < (*maskSize); ++(*p)) {
				(*q) = (*p) - (*maskMiddle) + (*j);
				if((*q) < 0 || (*q) >= (*original)->height) {
					(*isOut) = 1;
					(*outBound) += (*mask)[(*p)];
					continue;
				}
				(*sum) += (*mask)[(*p)]*(*original)->pic[(*q)][(*i)];
			}
			if((*isOut))
				(*sum) *= 1.0/(1.0-(*outBound));
			(*outpicY)[(*j)][(*i)] = (*sum);
		}
	}

}
void gaussianConvolution2DFast_loop_22(int *i, Image * *original, int *j, double *sum, int *isOut, double *outBound, int *p, int *maskSize, int *q, int *maskMiddle, double * *mask, double ** *outpicY, Image * *outImage)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			(*sum) = 0.0;
			(*isOut) = 0;
			(*outBound) = 0.0;
			for((*p) = 0; (*p) < (*maskSize); ++(*p)) {
				(*q) = (*p) - (*maskMiddle) + (*j);
				if((*q) < 0 || (*q) >= (*original)->width) {
					(*isOut) = 1;
					(*outBound) += (*mask)[(*p)];
					continue;
				}
				(*sum) += (*mask)[(*p)]*(*outpicY)[(*i)][(*q)];
			}
			if((*isOut))
				(*sum) *= 1.0/(1.0-(*outBound));
			(*outImage)->pic[(*i)][(*j)] = (*sum);
		}
	}

}
void gaussianConvolution2DFast_loop_23(int *i, Image * *original, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		free((*outpicY)[(*i)]);
	}

}
