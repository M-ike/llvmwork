#include "test_loop_2.h"






{1,'c'},
{1,'h'}
};



void swap(struct st *s1,Node *no);

#define N \
         100
int main(int argc, char **argv) {

#define N1 100
    int i,sum = 0;
    struct stu s;
    int inc;
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
    
    main_loop_1(&inc, &s, &sum, &jj, &argc);
/*This is
a test*/


    main_loop_2(&i, &sum);


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
