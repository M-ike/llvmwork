#include "sort_loop_1.h"
void merge_loop_1(int *j, int *start, int *k, int *end, double * *table, double * *sorted_table)
{
for((*j) = (*start), (*k) = 0; (*j) <= (*end); ++(*j), ++(*k)) {
		(*table)[(*j)] = (*sorted_table)[(*k)];
	}

}
