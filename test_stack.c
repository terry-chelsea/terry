#include <stdio.h>
#include "stack.h"


int main()
{
    int ar[] = {1 , 3 , 5 , 7 , 9 , 2 , 4 , 6 , 8 , 10};

    int size = sizeof(ar) / sizeof(ar[0]);
    Stack *st = create_stack(ar , size);
    if(NULL == st)
    {
        printf("Create failed !\n");
        return -1;
    }

    printf("MAX value : %d and MIN value : %d\n" , stack_max(st) , stack_min(st));
    while(!stack_empty(st))
    {
        printf("Element : %d\n" , stack_pop(st));
    }

    int i = 0 ;
    for(i = 0 ; i < size ; ++ i)
        stack_push(st , ar[i]);

    printf("MAX value : %d and MIN value : %d\n" , stack_max(st) , stack_min(st));

    stack_reverse(st);
    while(!stack_empty(st))
    {
        printf("Element : %d\n" , stack_pop(st));
    }

    destory_stack(st);

    return 0;
}
