#include<stdio.h>
#include<stdlib.h>

int ah;
#define JJ
#define AA 1
#ifdef JJ    //hhhhhhh
    #ifndef KK  /*helldsf
dslafjdklsjafldjslkjfakdsjlafjdklsj    
    kldjs*/
        #include"ifdef1.h"
        int b = 1 ;
    #endif 
#else
    #define N 20
    int b =2;
#endif


int main(){
    int i,a;

    //add50(&a);
    a = N;
    for(i=0;i< 50;i++)
        a++;
    printf("%d\n",a);
}
