#include "main_loop_4.h"
int main_loop_1(KeyPoint * *keyCounter, KeyPoint * *keypoints, int *totalKeypoints)
{
for((*keyCounter) = (*keypoints); (*keyCounter) != NULL; (*keyCounter)=(*keyCounter)->next) {
		++(*totalKeypoints);
	}

return 0;
}
int main_loop_2(KeyPoint * *keyCounter, KeyPoint * *keypoints, FILE * *output)
{
for((*keyCounter) = (*keypoints); (*keyCounter) != NULL; (*keyCounter)=(*keyCounter)->next) {
		printKeyPoint((*keyCounter), (*output));
	}

return 0;
}
int printTrashed_loop_3(KeyPoint * *counter, KeyPoint * *trash, FILE * *output, double *scale)
{
for((*counter) = (*trash); (*counter) != NULL; (*counter) = (*counter)->next) {
		fprintf((*output), "%4d %4d %5d %2.3f %s\n", (int) ((*counter)->y*(*scale)), (int) ((*counter)->x*(*scale)), (*counter)->imageLevel, (*counter)->relativeScale, (*counter)->description);
	}

return 0;
}
int printPoints_loop_4(KeyPoint * *counter, KeyPoint * *peaks, FILE * *output, double *scale)
{
for((*counter) = (*peaks); (*counter) != NULL; (*counter) = (*counter)->next) {
		fprintf((*output), "%4d %4d %5d %2.3f %s\n", (int) ((*counter)->y*(*scale)), (int) ((*counter)->x*(*scale)), (*counter)->imageLevel, (*counter)->relativeScale, (*counter)->description);
	}

return 0;
}
