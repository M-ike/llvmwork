#include<stdio.h>

void print(register int *a){
    printf("%d\n",*a);

    (*a)++;
}

int main()
{
    register int a;

    a =1;

   print(&a);
   printf("%d\n",a);
}
