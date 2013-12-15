#ifndef test_loop_1_h_
#define test_loop_1_h_

#include "stdio.h"

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
       };
typedef struct node{
  int a;
  char ch;
} Node;
void main_loop_1(int *i, struct stu *s, int *sum, int *jj, int *argc);

#endif
