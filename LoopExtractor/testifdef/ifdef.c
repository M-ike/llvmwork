#include "ifdef_loop_1.h"
int ah;
#define JJ
#define AA 1
#ifdef JJ    //hhhhhhh
    #ifndef KK  /*helldsf
dslafjdklsjafldjslkjfakdsjlafjdklsj    
    kldjs*/
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
    
    main_loop_1(&i, &a);

    printf("%d\n",a);
}
