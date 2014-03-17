#include "test_loop_1.h"
void print(register int a){
    printf("%d\n",a);
    a++;
}
int main(){
    register int a;
    a=1;
    int i;
    
    int register_1a = a;
main_loop_1(&i, &register_1a);
a = register_1a;


    print(a);
    printf("%d/n",a);
}
