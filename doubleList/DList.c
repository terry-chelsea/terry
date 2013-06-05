/*
 * =====================================================================================
 *
 *       Filename:  DList.c
 *
 *    Description:  implement of double list...
 *
 *        Version:  1.0
 *        Created:  07/02/12 23:52:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "DList.h"

LIST *DList_create()
{
    LIST *list = NULL;
    list = (LIST *)malloc(sizeof(LIST));
    assert(list != NULL);

    NODE *node = NULL;
    node = (NODE *)malloc(sizeof(NODE));
    assert(node != NULL);

    node->prev = node->next = NULL;
    node->data = malloc(sizeof(int));
    assert(node->data != NULL);
    *(int *)(node->data) = 0;

    list->head = list->rear = node;

    return list;
}

void DList_foreach(LIST *list , DListFunc func , void *data)
{
    assert(list != NULL && list->head != NULL);
    NODE *every_node = list->head->next;
    
    while(every_node != NULL)
    {
        func(data , every_node);
        every_node = every_node->next;
    }
}

void DList_destory(LIST *list)
{
    assert(list != NULL && list->head != NULL);
    
    NODE *node = list->head->next;
    NODE *temp = NULL;
    while(node != NULL)
    {
        temp = node->next;
        free(node->data);
        free(node);
        node = temp;
    }

    printf("there still have %d item in this list..\n" , *(int *)(list->head->data));
    free(list->head->data);
    free(list->head);
    free(list);
}

void DList_append(LIST *list , void *data)
{
    assert(data != NULL);
    assert(list != NULL && list->rear != NULL);

    NODE *node = NULL;
    node = (NODE *)malloc(sizeof(NODE));
    assert(node != NULL);
    node->data = data;

    node->prev = list->rear;
    node->next = NULL;
    list->rear->next = node;
    list->rear = node;

    (*(int *)(list->head->data)) ++;
}

void *DList_delete(LIST *list)
{
    assert(list != NULL && list->head != NULL);

    NODE *node = list->head->next;
    if(NULL == node)
    {
        assert(0 == *(int *)list->head->data);
        printf("list is empty , cannot delete item...\n");
        return NULL;
    }
    
    list->head->next = node->next;
    if(node->next != NULL)
        node->next->prev = list->head;
    else
        list->rear = list->head;

    void *data = node->data;
    free(node);
    
    (*(int *)(list->head->data)) --;

    return data;
}

void *DList_getHead(LIST *list)
{
    assert(list != NULL && list->head != NULL);

    if(NULL == list->head->next)
    {
        assert(0 == *(int *)(list->head->data));
        printf("list is empty ...\n");
        return NULL;
    }

    return list->head->next->data;
}

void *DList_getRear(LIST *list)
{
    assert(list != NULL);

    if(list->head == list->rear)
    {
        assert(0 == *(int *)(list->head->data));
        printf("list is empty...\n");
        return NULL;
    }

    return list->rear->data;
}

int DList_size(LIST *list)
{
    assert(list != NULL && list->head != NULL);

    return *(int *)(list->head->data);

}

