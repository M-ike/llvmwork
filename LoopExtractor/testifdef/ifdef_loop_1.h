#ifndef ifdef_loop_1_h_
#define ifdef_loop_1_h_

#include<stdio.h>
#include<stdlib.h>


#ifndef JJ
#define JJ

#endif

#ifdef JJ    //hhhhhhh
    #ifndef KK  /*helldsf
dslafjdklsjafldjslkjfakdsjlafjdklsj    
    kldjs*/
        #include"ifdef1.h"
    #endif 
#else
#endif

void main_loop_1(int *i, int *a);

#endif
