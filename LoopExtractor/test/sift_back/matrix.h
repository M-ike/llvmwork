/*
* Jan Prokaj
* May 11, 2005
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Matrix {
	int    rows;
	int    columns;
	double **values;
};
typedef struct Matrix Matrix;

void negate(Matrix *this);

double dot(Matrix *a, Matrix *b);

#endif
