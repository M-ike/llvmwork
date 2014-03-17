#include "ifdef_loop_1.h"
#ifndef JJ
#define JJ

#endif

#ifndef AA
#define AA 1

#endif

void main_loop_1(int *i, int *a)
{
for((*i)=0;(*i)< 50;(*i)++)
        (*a)++;

}
