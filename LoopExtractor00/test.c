#include "test_loop_1.h"


#ifndef NO
#define NO 10
#endif





void swap(struct st *s1,Node *no);

#define N \
         100
int main(int argc, char **argv) {

#define N1 100
    int sum = 0;
    struct stu s;
    int i;
    union Union u;
    u.a= 1;
    u.c = ' ';
    struct st s1;
    s1.b =0;
    s1.d = 'c';
    Node no;
    no.a=1;
    no.ch='a';
    swap(&s1,&no);
    int jj = 1;
    sum++;

    main_loop_1(&i, &s, &sum, &jj, &argc);
/*This is
a test*/

    return sum;
#undef N1
}

void swap(struct st *s1,Node *no){
    int temp;
    temp = (*s1).b;
    (*s1).b = (*no).a;
    (*no).a = temp;
    return;
}
