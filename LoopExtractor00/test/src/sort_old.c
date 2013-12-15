/*
*  Jan Prokaj
*  March 5, 2005
*/
#include "sort.h"

void mergesort(double *table, int start, int end)
{
	int k;

	if(start == end)
		return;

	k = start + (end - start)/2;
	mergesort(table, start, k);
	mergesort(table, k + 1, end);

	merge(table, start, k, end);
}

void merge(double *table, int start, int mid, int end)
{
	int    i = start;
	int    j = mid+1;
	int    k = 0;
	double *sorted_table;

	sorted_table = (double *) malloc(sizeof(double)*(end-start+1));
	while(i <= mid && j <= end) {
		if(table[i] < table[j]) {
				sorted_table[k] = table[i];
				++i;
		} else {
			sorted_table[k] = table[j];
			++j;
		}
		++k;
	}
	if(i <= mid) {
		while (i <= mid) {
			sorted_table[k] = table[i];
			++i;
			++k;
		}
	} else if(j <= end) {
		while(j <= end) {
			sorted_table[k] = table[j];
			++j;
			++k;
		}
	}
	for(j = start, k = 0; j <= end; ++j, ++k) {
		table[j] = sorted_table[k];
	}
	free(sorted_table);
}
