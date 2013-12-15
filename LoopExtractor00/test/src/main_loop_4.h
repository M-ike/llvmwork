#ifndef main_loop_4_h_
#define main_loop_4_h_

#include <stdio.h>
#include <stdlib.h>
#include "siftconstants.h"
#include "siftfilters.h"
#include "image.h"
#include "imageio.h"
#include "gaussconv.h"
#include "scalespace.h"
#include "octave.h"

void printPoints_loop_4(KeyPoint * *counter, KeyPoint * *peaks, FILE * *output, double *scale);
void main_loop_1(KeyPoint * *keyCounter, KeyPoint * *keypoints, int *totalKeypoints);
void main_loop_2(KeyPoint * *keyCounter, KeyPoint * *keypoints, FILE * *output);
void printTrashed_loop_3(KeyPoint * *counter, KeyPoint * *trash, FILE * *output, double *scale);

#endif
