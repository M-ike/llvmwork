#include<stdio.h>

void print(register int a){
    printf("%d\n",a);
    a++;
}
int main(){
    register int a;
    a=1;
    int i;
    for( i = 1; i < 50; i++ ){
        a++;
    }
    print(a);
    printf("%d/n",a);
}
