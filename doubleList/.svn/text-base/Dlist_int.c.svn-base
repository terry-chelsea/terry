/*
 * =====================================================================================
 *
 *       Filename:  Dlist_int.c
 *
 *    Description:  use int to test Double list...
 *
 *        Version:  1.0
 *        Created:  07/03/12 00:33:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "DList.h"
#include<stdio.h>
#include<assert.h>

int *new_int(int n)
{
    int *p = malloc(sizeof(int));
    assert(p != NULL);

    *p = n;

    return p;
}

void print_int(void *data , NODE *node)
{
    printf("%4d" , *(int *)(node->data));
}

int main()
{
    LIST *list = DList_create();

    int i = 0;
    for(i = 0 ; i < 10 ; ++ i)
    {
        int *p = new_int(i * 10 - 5);

        printf("append %d\n" , *p);
        DList_append(list , p);
        
        if(i % 3 == 0)
        {
            int *q = (int *)DList_delete(list);
            printf("delete %d\n" , *q);
            free(q);
        }
    }

    printf("list for each : \n");
    DList_foreach(list , print_int , NULL);
    printf("\n");
    printf("list size is %d\n" , DList_size(list));

    printf("list get head is %d\n" , *(int *)DList_getHead(list));
    printf("list get rear is %d\n" , *(int *)DList_getRear(list));

    DList_destory(list);

    return 0;
}
