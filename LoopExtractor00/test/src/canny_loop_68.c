#include "canny_loop_68.h"
void getCannyEdgesClassic_loop_1(int *i, int *maskSize, double ** *maskX, double ** *maskY)
{
for((*i) = 0; (*i) < (*maskSize); ++(*i)) {
		(*maskX)[(*i)] = (double *) malloc((*maskSize)*sizeof(double));
		(*maskY)[(*i)] = (double *) malloc((*maskSize)*sizeof(double));
	}

}
void getCannyEdgesClassic_loop_2(int *i, Image * *original, double ** *outpicX, double ** *outpicY)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		(*outpicX)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
		(*outpicY)[(*i)] = (double *) malloc((*original)->width*sizeof(double));
	}

}
void getCannyEdgesClassic_loop_3(int *i, Image * *outImage)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		(*outImage)->pic[(*i)] = (float *) malloc((*outImage)->width*sizeof(float));
	}

}
void getCannyEdgesClassic_loop_4(int *p, int *mr, int *q, double *maskval, double *normalizeFactor, float *sigma, double ** *maskX, int *centY, int *centX, double ** *maskY)
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
void getCannyEdgesClassic_loop_5(int *i, int *mr, Image * *original, int *j, double *sum1, double *sum2, int *p, int *q, double ** *maskX, int *centY, int *centX, double ** *maskY, double ** *outpicX, double ** *outpicY)
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
void getCannyEdgesClassic_loop_6(int *i, Image * *original, int *j, Image * *outImage, int *mr, double ** *outpicX, double ** *outpicY)
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
void getCannyEdgesClassic_loop_7(int *i, Image * *outImage, int *j, float *max)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*outImage)->width; ++(*j)) {
			if((*outImage)->pic[(*i)][(*j)] > (*max))
				(*max) = (*outImage)->pic[(*i)][(*j)];
		}
	}

}
void getCannyEdgesClassic_loop_8(int *i, Image * *outImage, int *j, float *max)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*outImage)->width; ++(*j)) {
			(*outImage)->pic[(*i)][(*j)] /= (*max);
		}
	}

}
void getCannyEdgesClassic_loop_9(int *i, Image * *peaks)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		(*peaks)->pic[(*i)] = (float *) malloc((*peaks)->width*sizeof(float));
	}

}
void getCannyEdgesClassic_loop_10(int *i, Image * *peaks, int *j, float *xDiff, double ** *outpicX, float *yDiff, double ** *outpicY, double *angle, Image * *outImage)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		for ((*j) = 0; (*j) < (*peaks)->width; ++(*j)) {
			(*peaks)->pic[(*i)][(*j)] = 0.0;
			if((*i) == 0 || (*j) == 0 || (*i) == (*peaks)->height - 1 || (*j) == (*peaks)->width - 1) {
				continue;
			}
			(*xDiff) = (*outpicX)[(*i)][(*j)];
			(*yDiff) = (*outpicY)[(*i)][(*j)];
			if((*xDiff) == 0)
				(*xDiff) = 0.00000001;
			(*angle) = atan((*yDiff)/(*xDiff));
			if(((*angle) >= -M_PI / 8.0) && ((*angle) < M_PI / 8.0)) {
				if(((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)][(*j)+1]) &&
				    ((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else if(((*angle) >= M_PI / 8.0) && ((*angle) < 3.0*M_PI / 8.0)) {
				if(((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)+1][(*j)+1]) && ((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)-1][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else if(((*angle) >= -3.0*M_PI / 8.0) && ((*angle) < -M_PI / 8.0)) {
				if(((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)-1][(*j)+1]) && ((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)+1][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else {
				if(((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)+1][(*j)]) && ((*outImage)->pic[(*i)][(*j)] > (*outImage)->pic[(*i)-1][(*j)])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
		}
	}

}
void getCannyEdgesClassic_loop_11(int *i, Image * *outImage, int *j, int hist[])
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
	   for((*j) = 0; (*j) < (*outImage)->width; ++(*j)) {
	   	//printf("%d ", (int) ((*outImage)->pic[(*i)][(*j)]*255));
	      hist[(int) ((*outImage)->pic[(*i)][(*j)]*255)]++;
	   }
	}

}
void getCannyEdgesClassic_loop_12(int *i, int *sum, int hist[], float *picPercent, float *HI)
{
for((*i) = 255; (*i) >= 0; --(*i)) {
	   (*sum) += hist[(*i)];
	   if((*sum) >= (*picPercent)) {
	      (*HI) = (*i);
	      break;
	   }
	}

}
void getCannyEdgesClassic_loop_13(int *i, Image * *canny)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		(*canny)->pic[(*i)] = (float *) malloc((*canny)->width*sizeof(float));
	}

}
void getCannyEdgesClassic_loop_14(int *i, Image * *canny, int *j)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*canny)->width; ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdgesClassic_loop_15(int *i, Image * *outImage, int *j, Image * *peaks, float *HI, Image * *canny, float *LO)
{
for((*i) = 0; (*i) < (*outImage)->height; ++(*i)) {
	   for((*j) = 0; (*j) < (*outImage)->width; ++(*j)) {
	      if((*peaks)->pic[(*i)][(*j)] == 1.0) {
	         if((*outImage)->pic[(*i)][(*j)] > ((*HI)/255)) {
		    (*peaks)->pic[(*i)][(*j)] = 0.0;
		    (*canny)->pic[(*i)][(*j)] = 1.0;
		 }
		 else if((*outImage)->pic[(*i)][(*j)] < ((*LO)/255)) {
		    (*peaks)->pic[(*i)][(*j)] = 0.0;
		 }
	      }
	   }
	}

}
void getCannyEdgesClassic_loop_16(int *i, Image * *peaks, int *j, Image * *canny)
{
for((*i) = 1; (*i) < (*peaks)->height - 1; ++(*i)) {
		for((*j) = 1; (*j) < (*peaks)->width - 1; ++(*j)) {
			if((*peaks)->pic[(*i)][(*j)] == 1.0) {
				traceRidge((*peaks), (*canny), (*i), (*j));
			}
		}
	}

}
void getCannyEdgesClassic_loop_17(int *i, int *mr, int *j, Image * *canny)
{
for((*i) = 0; (*i) <= (*mr); ++(*i)) {
		for((*j) = 0; (*j) < (*canny)->width; ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdgesClassic_loop_18(int *i, Image * *canny, int *mr, int *j)
{
for((*i) = (*canny)->height - (*mr); (*i) < (*canny)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*canny)->width; ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdgesClassic_loop_19(int *i, Image * *canny, int *j, int *mr)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		for((*j) = 0; (*j) <= (*mr); ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdgesClassic_loop_20(int *i, Image * *canny, int *j, int *mr)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		for((*j) = (*canny)->width - (*mr); (*j) < (*canny)->width; ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdgesClassic_loop_21(int *i, Image * *peaks, Image * *outImage)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		free((*peaks)->pic[(*i)]);
		free((*outImage)->pic[(*i)]);
		//free anglesource stuff
	}

}
void getCannyEdges_loop_22(int *i, Image * *original, int *j, float ** *xDeriv, float ** *yDeriv)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
			for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
				(*original)->pic[(*i)][(*j)] = (float) sqrt((*xDeriv)[(*i)][(*j)]*(*xDeriv)[(*i)][(*j)] + (*yDeriv)[(*i)][(*j)]*(*yDeriv)[(*i)][(*j)]);
			}
		}

}
void getCannyEdges_loop_23(int *i, Image * *original, int *j, float *max)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			if((*original)->pic[(*i)][(*j)] > (*max))
				(*max) = (*original)->pic[(*i)][(*j)];
		}
	}

}
void getCannyEdges_loop_24(int *i, Image * *original, int *j, float *max)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			(*original)->pic[(*i)][(*j)] /= (*max);
		}
	}

}
void getCannyEdges_loop_25(int *i, Image * *peaks)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		(*peaks)->pic[(*i)] = (float *) malloc((*peaks)->width*sizeof(float));
	}

}
void getCannyEdges_loop_26(int *i, Image * *peaks, int *j, Image * *angleSource, float *xDiff, float ** *xDeriv, float *yDiff, float ** *yDeriv, double *angle, Image * *original)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		for ((*j) = 0; (*j) < (*peaks)->width; ++(*j)) {
			(*peaks)->pic[(*i)][(*j)] = 0.0;
			if((*i) == 0 || (*j) == 0 || (*i) == (*peaks)->height - 1 || (*j) == (*peaks)->width - 1) {
				continue;
			}
			if((*angleSource) == NULL) {
				(*xDiff) = (*xDeriv)[(*i)][(*j)];
				(*yDiff) = (*yDeriv)[(*i)][(*j)];
			}
			else {
				(*xDiff) = (*angleSource)->pic[(*i)][(*j)+1] - (*angleSource)->pic[(*i)][(*j)-1];
				(*yDiff) = (*angleSource)->pic[(*i)-1][(*j)] - (*angleSource)->pic[(*i)+1][(*j)];
			}
			if((*xDiff) == 0)
				(*xDiff) = 0.00000001;
			(*angle) = atan((*yDiff)/(*xDiff));
			if(((*angle) >= -M_PI / 8.0) && ((*angle) < M_PI / 8.0)) {
				if(((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)][(*j)+1]) &&
				    ((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else if(((*angle) >= M_PI / 8.0) && ((*angle) < 3.0*M_PI / 8.0)) {
				if(((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)-1][(*j)+1]) && ((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)+1][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else if(((*angle) >= -3.0*M_PI / 8.0) && ((*angle) < -M_PI / 8.0)) {
				if(((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)+1][(*j)+1]) && ((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)-1][(*j)-1])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
			else {
				if(((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)+1][(*j)]) && ((*original)->pic[(*i)][(*j)] > (*original)->pic[(*i)-1][(*j)])) {
					(*peaks)->pic[(*i)][(*j)] = 1.0;
				}
			}
		}
	}

}
void getCannyEdges_loop_27(int *i, Image * *original, int *j, int hist[])
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			//printf("%d ", (int) ((*original)->pic[(*i)][(*j)]*255));
			hist[(int) ((*original)->pic[(*i)][(*j)]*255)]++;
		}
	}

}
void getCannyEdges_loop_28(int *i, int *sum, int hist[], float *picPercent, float *HI)
{
for((*i) = 255; (*i) >= 0; --(*i)) {
		(*sum) += hist[(*i)];
		if((*sum) >= (*picPercent)) {
			(*HI) = (*i);
			break;
		}
	}

}
void getCannyEdges_loop_29(int *i, Image * *canny)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		(*canny)->pic[(*i)] = (float *) malloc((*canny)->width*sizeof(float));
	}

}
void getCannyEdges_loop_30(int *i, Image * *canny, int *j)
{
for((*i) = 0; (*i) < (*canny)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*canny)->width; ++(*j)) {
			(*canny)->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void getCannyEdges_loop_31(int *i, Image * *original, int *j, Image * *peaks, float *HI, Image * *canny, float *LO)
{
for((*i) = 0; (*i) < (*original)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*original)->width; ++(*j)) {
			if((*peaks)->pic[(*i)][(*j)] == 1.0) {
				if((*original)->pic[(*i)][(*j)] > ((*HI)/255)) {
					(*peaks)->pic[(*i)][(*j)] = 0.0;
					(*canny)->pic[(*i)][(*j)] = 1.0;
				}
				else if((*original)->pic[(*i)][(*j)] < ((*LO)/255)) {
					(*peaks)->pic[(*i)][(*j)] = 0.0;
				}
			}
		}
	}

}
void getCannyEdges_loop_32(int *i, Image * *peaks, int *j, Image * *canny)
{
for((*i) = 1; (*i) < (*peaks)->height - 1; ++(*i)) {
		for((*j) = 1; (*j) < (*peaks)->width - 1; ++(*j)) {
			if((*peaks)->pic[(*i)][(*j)] == 1.0) {
				traceRidge((*peaks), (*canny), (*i), (*j));
			}
		}
	}

}
void getCannyEdges_loop_33(int *i, Image * *peaks, Image * *original)
{
for((*i) = 0; (*i) < (*peaks)->height; ++(*i)) {
		free((*peaks)->pic[(*i)]);
		free((*original)->pic[(*i)]);
		//free anglesource stuff
	}

}
void newCannyPPM_loop_34(int *i, int *gSize, double ** *gaussianx)
{
for((*i) = 0; (*i) < (*gSize); ++(*i)) {
		(*gaussianx)[(*i)] = (double *) malloc((*gSize)*sizeof(double));
	}

}
void newCannyPPM_loop_35(int *i, int *gSize, double ** *gaussiany)
{
for((*i) = 0; (*i) < (*gSize); ++(*i)) {
		(*gaussiany)[(*i)] = (double *) malloc((*gSize)*sizeof(double));
	}

}
void newCannyPPM_loop_36(int *i, int *gSize, int *q, int *j, int *p, double ** *gaussianx, double *kappa, double *sigma, double ** *gaussiany)
{
for((*i) = -(*gSize)/2; (*i) < (*gSize)/2 + 1; ++(*i)) {
		(*q) = (*i) + (*gSize)/2;
		for((*j) = -(*gSize)/2; (*j) < (*gSize)/2 + 1; ++(*j)) {
			(*p) = (*j) + (*gSize)/2;
                
			(*gaussianx)[(*q)][(*p)] = (-(*kappa)*(*j)/((*sigma)*(*sigma)))*exp((-(*j)*(*j) - (*i)*(*i))/(2*(*sigma)*(*sigma)));
			(*gaussiany)[(*q)][(*p)] = (-(*kappa)*(*i)/((*sigma)*(*sigma)))*exp((-(*j)*(*j) - (*i)*(*i))/(2*(*sigma)*(*sigma)));
		}
	}

}
void newCannyPPM_loop_37(int *i, Canny * *new)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*new)->magnitude[(*i)] = (double *) malloc((*new)->width*sizeof(double));
	}

}
void newCannyPPM_loop_38(int *i, Canny * *new)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*new)->theta[(*i)] = (double *) malloc((*new)->width*sizeof(double));
	}

}
void newCannyPPM_loop_39(int *y, Canny * *new, int *x, double *dgxSum, double *dgySum, double *xrSum, double *yrSum, double *xgSum, double *ygSum, double *xbSum, double *ybSum, int *i, int *gSize, int *q, int *j, int *p, ImageRGB * *image, double ** *gaussianx, double ** *gaussiany, double *rMag, double *gMag, double *bMag)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			(*dgxSum) = 0.0;
			(*dgySum) = 0.0;
			(*xrSum) = 0.0;
			(*yrSum) = 0.0;
			(*xgSum) = 0.0;
			(*ygSum) = 0.0;
			(*xbSum) = 0.0;
			(*ybSum) = 0.0;
                
			for((*i) = -(*gSize)/2; (*i) < (*gSize)/2 + 1; ++(*i)) {
				(*q) = (*i) + (*gSize)/2;
				for((*j) = -(*gSize)/2; (*j) < (*gSize)/2 + 1; ++(*j)) {
					(*p) = (*j) + (*gSize)/2;
					if((*x)+(*j) >= 0 && 
					   (*x)+(*j) < (*new)->width && 
					   (*y)+(*i) >= 0 && 
					   (*y)+(*i) < (*new)->height) {
						(*xrSum) += (*image)->red->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussianx)[(*q)][(*p)];
						(*yrSum) += (*image)->red->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussiany)[(*q)][(*p)];
						(*xgSum) += (*image)->green->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussianx)[(*q)][(*p)];
						(*ygSum) += (*image)->green->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussiany)[(*q)][(*p)];
						(*xbSum) += (*image)->blue->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussianx)[(*q)][(*p)];
						(*ybSum) += (*image)->blue->pic[(*y)+(*i)][(*x)+(*j)] * (*gaussiany)[(*q)][(*p)];
						(*dgxSum) += (*gaussianx)[(*q)][(*p)];
						(*dgySum) += (*gaussiany)[(*q)][(*p)];
					}
				}
			}
                
			(*xrSum) = (*xrSum) / (*dgxSum);
			(*yrSum) = (*yrSum) / (*dgySum);
			(*xgSum) = (*xgSum) / (*dgxSum);
			(*ygSum) = (*ygSum) / (*dgySum);
			(*xbSum) = (*xbSum) / (*dgxSum);
			(*ybSum) = (*ybSum) / (*dgySum);
			
			(*rMag) = sqrt((*xrSum)*(*xrSum) + (*yrSum)*(*yrSum));
			(*gMag) = sqrt((*xgSum)*(*xgSum) + (*ygSum)*(*ygSum));
			(*bMag) = sqrt((*xbSum)*(*xbSum) + (*ybSum)*(*ybSum));
			
			(*new)->magnitude[(*y)][(*x)] = max3((*rMag), (*gMag), (*bMag));
			if((*new)->magnitude[(*y)][(*x)] == (*rMag))
				(*new)->theta[(*y)][(*x)] = atan2((*yrSum), (*xrSum));
			else if((*new)->magnitude[(*y)][(*x)] == (*gMag))
				(*new)->theta[(*y)][(*x)] = atan2((*ygSum), (*xgSum));
			else
				(*new)->theta[(*y)][(*x)] = atan2((*ybSum), (*xbSum));
		}
        }

}
void newCannyPPM_loop_40(int *i, Canny * *new, int *j, double *max)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			if((*new)->magnitude[(*i)][(*j)] > (*max))
				(*max) = (*new)->magnitude[(*i)][(*j)];
		}
	}

}
void newCannyPPM_loop_41(int *i, Canny * *new, int *j, double *max)
{
for((*i) = 0; (*i) < (*new)->height; (*i)++){
		for((*j) = 0; (*j) < (*new)->width; (*j)++){
			(*new)->magnitude[(*i)][(*j)] = (*new)->magnitude[(*i)][(*j)] / (*max);
		}
	}

}
void newCannyPPM_loop_42(int *i, int *gSize, double ** *gaussianx, double ** *gaussiany)
{
for((*i) = 0; (*i) < (*gSize); ++(*i)) {
		free((*gaussianx)[(*i)]);
		free((*gaussiany)[(*i)]);
	}

}
void newCannyPPM_loop_43(int *i, Canny * *new, int ** *peaks)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*peaks)[(*i)] = (int *) malloc((*new)->width*sizeof(int));
	}

}
void newCannyPPM_loop_44(int *y, Canny * *new, int *x, int ** *peaks, int *sector)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			(*peaks)[(*y)][(*x)] = 0;
                
			/* boundary condition */
			if((*y) < 1 || (*y) >= (*new)->height - 1 ||
			   (*x) < 1 || (*x) >= (*new)->width - 1)
			   	continue;
			
			// get the "sector" of the pixel (this corresponds to its gradient's direction
			(*sector) = sectorAngle((*new), (*x), (*y));
                
			// for each (*sector), check the adjacent pixels
			// horizontal case
			if((*sector) == 0) {
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)+1])
						(*peaks)[(*y)][(*x)] = 255;
                        	} 
				else if ((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)][(*x)-1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)][(*x)+1] -= 0.0001;
					}
				}
			}
			
			// bottom to top diagonal case
			if((*sector) == 1){
				//if((*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)+1][(*y)+1] && (*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)-1][(*y)-1])
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)-1])
						(*peaks)[(*y)][(*x)] = 255;
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)-1][(*x)+1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)-1][(*x)+1] -= 0.0001;
					}
				}
			}
			
			// vertical case
			if((*sector) == 2) {
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)]) {
						(*peaks)[(*y)][(*x)] = 255;
					}
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)+1][(*x)]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)+1][(*x)] -= 0.0001;
					}
				}
			}
			
			// top to bottom diagonal case
			if((*sector) == 3){
				//if((*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)+1][(*y)-1] && (*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)-1][(*y)+1])
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)-1])
						(*peaks)[(*y)][(*x)] = 255;
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)+1][(*x)+1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)+1][(*x)+1] -= 0.0001;
					}
				}
			}
		}
	}

}
void newCannyPPM_loop_45(int *y, Canny * *new, int *x, int ** *peaks, double *low)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*peaks)[(*y)][(*x)] == 255 && (*new)->magnitude[(*y)][(*x)] < (*low))
				(*new)->magnitude[(*y)][(*x)] = 0.0;
		}
        }

}
void newCannyPPM_loop_46(int *y, Canny * *new, int *x, int ** *peaks, double *high)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*peaks)[(*y)][(*x)] == 255){
				if((*new)->magnitude[(*y)][(*x)] >= (*high)) {
					(*new)->edges->pic[(*y)][(*x)] = 1.0;
					//magnitude[(*x)][(*y)] = 0;
				}
				else {
					(*new)->edges->pic[(*y)][(*x)] = 0.0;
				}
			}
		}
	}

}
void newCannyPPM_loop_47(int *i, Canny * *new, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*traced)[(*i)] = (int *) malloc((*new)->width*sizeof(int));
	}

}
void newCannyPPM_loop_48(int *i, Canny * *new, int *j, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*traced)[(*i)][(*j)] = 0;
		}
	}

}
void newCannyPPM_loop_49(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*new)->edges->pic[(*y)][(*x)] == 1.0 && !(*traced)[(*y)][(*x)])
				traceColor((*new), (*x), (*y), (*low), (*traced), (*peaks));
		}
	}

}
void newCannyPPM_loop_50(int *j, int *border, int *i, Canny * *new)
{
for((*j) = 0; (*j) <= (*border); ++(*j)) {
		for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
			(*new)->edges->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void newCannyPPM_loop_51(int *j, Canny * *new, int *border, int *i)
{
for((*j) = (*new)->width - (*border); (*j) < (*new)->width; ++(*j)) {
		for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
			(*new)->edges->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void newCannyPPM_loop_52(int *i, int *border, int *j, Canny * *new)
{
for((*i) = 0; (*i) <= (*border); ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*new)->edges->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void newCannyPPM_loop_53(int *i, Canny * *new, int *border, int *j)
{
for((*i) = (*new)->height - (*border); (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*new)->edges->pic[(*i)][(*j)] = 0.0;
		}
	}

}
void newCannyPPM_loop_54(int *i, Canny * *new, int ** *peaks, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		free((*peaks)[(*i)]);
		free((*traced)[(*i)]);
	}

}
void newCannyPGMTwoSources_loop_55(int *i, Canny * *new)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*new)->magnitude[(*i)] = (double *) malloc((*new)->width*sizeof(double));
	}

}
void newCannyPGMTwoSources_loop_56(int *i, Canny * *new)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*new)->theta[(*i)] = (double *) malloc((*new)->width*sizeof(double));
	}

}
void newCannyPGMTwoSources_loop_57(int *i, Canny * *new, int *j, Image * *gradient, Image * *gaussian)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*new)->magnitude[(*i)][(*j)] = (*gradient)->pic[(*i)][(*j)];
			if((*j) > 0 && (*j) < (*new)->width - 1 && (*i) > 0 && (*i) < (*new)->height - 1) {
				(*new)->theta[(*i)][(*j)] = atan2((*gaussian)->pic[(*i)-1][(*j)] - (*gaussian)->pic[(*i)+1][(*j)], (*gaussian)->pic[(*i)][(*j)+1] - (*gaussian)->pic[(*i)][(*j)-1]);
			}
		}
	}

}
void newCannyPGMTwoSources_loop_58(int *i, Canny * *new, int ** *peaks)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*peaks)[(*i)] = (int *) malloc((*new)->width*sizeof(int));
	}

}
void newCannyPGMTwoSources_loop_59(int *y, Canny * *new, int *x, int ** *peaks, int *sector)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			(*peaks)[(*y)][(*x)] = 0;
			
			/* boundary condition */
			if((*y) < 1 || (*y) >= (*new)->height - 1 ||
			   (*x) < 1 || (*x) >= (*new)->width - 1)
			   	continue;
                
			// get the "sector" of the pixel (this corresponds to its gradient's direction
			(*sector) = sectorAngle((*new), (*x), (*y));
                
			// for each (*sector), check the adjacent pixels
			// horizontal case
			if((*sector) == 0) {
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)+1])
						(*peaks)[(*y)][(*x)] = 255;
                        	} 
				else if ((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)][(*x)-1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)][(*x)+1] -= 0.0001;
					}
				}
			}
			
			// bottom to top diagonal case
			if((*sector) == 1){
				//if((*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)+1][(*y)+1] && (*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)-1][(*y)-1])
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)-1])
						(*peaks)[(*y)][(*x)] = 255;
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)-1][(*x)+1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)-1][(*x)+1] -= 0.0001;
					}
				}
			}
			
			// vertical case
			if((*sector) == 2){
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)])
						(*peaks)[(*y)][(*x)] = 255;
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)+1][(*x)]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)+1][(*x)] -= 0.0001;
					}
				}
			}
			
			// top to bottom diagonal case
			if((*sector) == 3){
				//if((*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)+1][(*y)-1] && (*new)->magnitude[(*x)][(*y)] > (*new)->magnitude[(*x)-1][(*y)+1])
				if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)+1][(*x)+1]) {
					if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)-1])
						(*peaks)[(*y)][(*x)] = 255;
				}
				else if((*new)->magnitude[(*y)][(*x)] > (*new)->magnitude[(*y)-1][(*x)-1]) {
					if((*new)->magnitude[(*y)][(*x)] == (*new)->magnitude[(*y)+1][(*x)+1]) {
						(*peaks)[(*y)][(*x)] = 255;
						(*new)->magnitude[(*y)+1][(*x)+1] -= 0.0001;
					}
				}
			}
		}
	}

}
void newCannyPGMTwoSources_loop_60(int *y, Canny * *new, int *x, int ** *peaks, double *low)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*peaks)[(*y)][(*x)] == 255 && (*new)->magnitude[(*y)][(*x)] < (*low))
				(*new)->magnitude[(*y)][(*x)] = 0.0;
		}
        }

}
void newCannyPGMTwoSources_loop_61(int *y, Canny * *new, int *x, int ** *peaks, double *high)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*peaks)[(*y)][(*x)] == 255){
				if((*new)->magnitude[(*y)][(*x)] >= (*high)) {
					(*new)->edges->pic[(*y)][(*x)] = 1.0;
					//magnitude[(*x)][(*y)] = 0;
				}
				else {
					(*new)->edges->pic[(*y)][(*x)] = 0.0;
				}
			}
		}
	}

}
void newCannyPGMTwoSources_loop_62(int *i, Canny * *new, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		(*traced)[(*i)] = (int *) malloc((*new)->width*sizeof(int));
	}

}
void newCannyPGMTwoSources_loop_63(int *i, Canny * *new, int *j, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		for((*j) = 0; (*j) < (*new)->width; ++(*j)) {
			(*traced)[(*i)][(*j)] = 0;
		}
	}

}
void newCannyPGMTwoSources_loop_64(int *y, Canny * *new, int *x, int ** *traced, double *low, int ** *peaks)
{
for((*y) = 0; (*y) < (*new)->height; ++(*y)) {
		for((*x) = 0; (*x) < (*new)->width; ++(*x)) {
			if((*new)->edges->pic[(*y)][(*x)] == 1.0 && !(*traced)[(*y)][(*x)])
				traceColor((*new), (*x), (*y), (*low), (*traced), (*peaks));
		}
	}

}
void newCannyPGMTwoSources_loop_65(int *i, Canny * *new, int ** *peaks, int ** *traced)
{
for((*i) = 0; (*i) < (*new)->height; ++(*i)) {
		free((*peaks)[(*i)]);
		free((*traced)[(*i)]);
	}

}
void findThresholds_loop_66(int *i, Canny * *this, int *j, int hist[])
{
for ((*i) = 0; (*i) < (*this)->height; ++(*i)) {
		for ((*j) = 0; (*j) < (*this)->width; ++(*j)) {
			hist[(int)((*this)->magnitude[(*i)][(*j)]*255)]++;
		}
	}

}
void findThresholds_loop_67(int *c, int *area, int hist[], double *percent, Canny * *this)
{
for((*c) = 255; (*c) >= 0; (*c)--){
		(*area) = (*area) + hist[(*c)];
		if (((*percent)*(*this)->width*(*this)->height) < (*area)){
			break;
		}
	}

}
void freeCanny_loop_68(int *i, Canny * *this)
{
for((*i) = 0; (*i) < (*this)->height; ++(*i)) {
		free((*this)->magnitude[(*i)]);
		free((*this)->theta[(*i)]);
	}

}
