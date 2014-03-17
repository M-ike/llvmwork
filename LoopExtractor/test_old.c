#include "stdio.h"

#define SUM(x) x++

#ifndef NO
#define NO 10
#endif


struct st {
         int b;
         char d;
        };

union Union{
	int a;
	char c;
};

struct stu {
         int a;
         char c;
} array[2]={
{1,'c'},
{1,'h'}
};
typedef struct node{
  int a;
  char ch;
} Node;

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
    for(inc = 0; inc < N1; inc++) {
        int y=s.a;
        int j;
        for(j = 0; j < N; j++) {
            j += 2;
        }
	//s1.b++;
        sum += jj;
        sum += argc;
        //no.a++;
        printf(" \"sum = %d\n", sum);
    }/*This is
a test*/

for(i = 0 ; i<100;i++)
 SUM(sum);

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
