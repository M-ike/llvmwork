#ifndef main_loop_4_h_
#define main_loop_4_h_

/*
*  Jan Prokaj
*  March 1, 2005
*  based on libsift-1.6 -- Sebastian Nowozin (nowozin@cs.tu-berlin.de)
*/
#include <stdio.h>
#include <stdlib.h>
#include "siftconstants.h"
#include "siftfilters.h"
#include "image.h"
#include "imageio.h"
#include "gaussconv.h"
#include "scalespace.h"
#include "octave.h"


int printTrashed_loop_3(KeyPoint * *counter, KeyPoint * *trash, FILE * *output, double *scale);
int main_loop_2(KeyPoint * *keyCounter, KeyPoint * *keypoints, FILE * *output);
int main_loop_1(KeyPoint * *keyCounter, KeyPoint * *keypoints, int *totalKeypoints);
int printPoints_loop_4(KeyPoint * *counter, KeyPoint * *peaks, FILE * *output, double *scale);

#endif
