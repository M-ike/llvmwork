#include "test_loop_1.h"
#ifndef NO
#define NO 10
#endif

#ifndef N
#define N \
         100
#endif

#ifndef N1
#define N1 100
#endif

void main_loop_1(int *i, struct stu *s, int *sum, int *jj, int *argc)
{
    for((*i) = 0; (*i) < N1; (*i)++) {
        int y=(*s).a;
        int j;
        for(j = 0; j < N; j++) {
            j += 2;
        }
	//s1.b++;
        (*sum) += (*jj);
        (*sum) += (*argc);
        //no.a++;
        printf(" \"sum = %d\n", (*sum));
    }
}
#ifdef N1
#undef N1
#endif

