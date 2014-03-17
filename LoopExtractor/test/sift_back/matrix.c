/*
* Jan Prokaj
* May 11, 2005
*/

#include "matrix.h"

//linear solve here

double dot(Matrix *a, Matrix *b) {

	double sum = 0.0;
	int    i;

	if (a->rows != b->rows || a->columns != 1 || b->columns != 1)
		printf("Dotproduct only valid for two equal n x 1 matrices\n");
	for (i = 0; i < a->rows; ++i)
		sum += a->values[i][0] * b->values[i][0];
	return sum;
}

void negate(Matrix *this) {

	int i, j;
	for(i = 0; i < this->rows; ++i)
		for(j = 0; j < this->columns; ++j)
			this->values[i][j] = -1*(this->values[i][j]);
}
