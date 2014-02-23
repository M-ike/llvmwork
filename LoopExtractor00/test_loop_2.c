#include "test_loop_2.h"
#ifndef SUM
#define SUM(x) x++

#endif

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

void main_loop_1(int *inc, struct stu *s, int *sum, int *jj, int *argc)
{
for((*inc) = 0; (*inc) < N1; (*inc)++) {
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
void main_loop_2(int *i, int *sum)
{
for((*i) = 0 ; (*i)<100;(*i)++)
 SUM((*sum));

}
#ifdef N1
#undef N1
#endif

