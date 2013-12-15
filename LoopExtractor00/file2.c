#include<stdio.h>

extern int a;
extern int b[];

void func(int *q);
int main(){
   int *p;
   *p =1;
    printf("%d %d\n",a,b[1]);
    func(p);
}

void func(int *q){
    printf("%d \n", *q);
    return;

}


