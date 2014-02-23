#include "matrix_loop_2.h"
double dot(Matrix *a, Matrix *b) {

	double sum = 0.0;
	int    i;

	if (a->rows != b->rows || a->columns != 1 || b->columns != 1)
		printf("Dotproduct only valid for two equal n x 1 matrices\n");
	
    dot_loop_1(&i, &a, &sum, &b);

	return sum;
}

void negate(Matrix *this) {

	int i, j;
	
    negate_loop_2(&i, &this, &j);

}
