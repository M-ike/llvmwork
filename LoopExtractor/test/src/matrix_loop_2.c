#include "matrix_loop_2.h"
int dot_loop_1(int *i, Matrix * *a, double *sum, Matrix * *b)
{
for ((*i) = 0; (*i) < (*a)->rows; ++(*i))
		(*sum) += (*a)->values[(*i)][0] * (*b)->values[(*i)][0];

return 0;
}
int negate_loop_2(int *i, Matrix * *this, int *j)
{
for((*i) = 0; (*i) < (*this)->rows; ++(*i))
		for((*j) = 0; (*j) < (*this)->columns; ++(*j))
			(*this)->values[(*i)][(*j)] = -1*((*this)->values[(*i)][(*j)]);

return 0;
}
