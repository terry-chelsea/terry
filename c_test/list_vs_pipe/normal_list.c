/*
 * =====================================================================================
 *
 *       Filename:  normal_list.c
 *
 *    Description:  this is a normal list only offer put and get operations 
 *    with mutil threads...
 *
 *        Version:  1.0
 *        Created:  07/09/13 17:04:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "normal_list.h"

LIST *list_create()
{
    LIST *lst = (LIST *)malloc(sizeof(LIST));
    if(NULL == lst)
        return NULL;

    memset(lst , 0 , sizeof(*lst));

    lst->head = lst->tail = NULL;
    lst->put_ops = 0;
    lst->get_ops = 0;
    lst->elems = 0;

#ifdef DOUBLE_LOCK
    if((pthread_mutex_init(&(lst->tail_mutex) , NULL) < 0) ||
            (pthread_mutex_init(&(lst->head_mutex) , NULL) < 0))
    {
        perror("Initialied double mutex failed : ");
        return NULL;
    }
#endif

    if((pthread_mutex_init(&(lst->lmutex) , NULL) < 0) || 
            (pthread_cond_init(&(lst->lcond) , NULL) < 0))
    {
        perror("init mutex or condtion failed : ");
        return NULL;
    }

    return lst;
}

#ifdef DOUBLE_LOCK 
//I need a CAS operation !!!
static void add_to_list_tail_with_double_lock(LIST *lst , NODE *node)
{
    pthread_mutex_lock(&(lst->head_mutex));
    if(NULL == lst->head)
        lst->head = node;
    pthread_mutex_unlock(&(lst->head_mutex));

    pthread_mutex_lock(&(lst->tail_mutex));
    if(NULL == lst->tail)
        lst->tail = node;
    else 
    {
        lst->tail->next = node;
        lst->tail = node;
    }
    pthread_mutex_unlock(&(lst->tail_mutex));
}

static NODE *del_from_list_head_with_double_lock(LIST *lst)
{
    NODE *node = NULL;
    pthread_mutex_lock(&(lst->tail_mutex));
    if(NULL == lst->tail)
    {
        pthread_mutex_unlock(&(lst->tail_mutex));
        return node;
    }
    pthread_mutex_lock(&(lst->head_mutex));
    if(lst->head == lst->tail)
    {
        node = lst->head;
        lst->head = NULL;
        lst->tail = NULL;
        
        pthread_mutex_unlock(&(lst->tail_mutex));
        pthread_mutex_unlock(&(lst->head_mutex));
        return node;
    }
    else
    {
        pthread_mutex_unlock(&(lst->tail_mutex));
        node = lst->head;
        lst->head = node->next;
        pthread_mutex_unlock(&(lst->head_mutex));

        return node;
    } 
}

#endif

static void add_to_list_tail(LIST *lst , NODE *node)
{
    if(NULL == lst->head)
    {
        lst->head = node;
        lst->tail = node;
    }
    else 
    {
        lst->tail->next = node;
        lst->tail = node;
    }
}

static NODE *del_from_list_head(LIST *lst)
{
    NODE *node = NULL;
    if(NULL == lst->head)
        node = NULL;
    else if(lst->head == lst->tail)
    {
        node = lst->head;
        lst->head = NULL;
        lst->tail = NULL;
    }
    else 
    {
        node = lst->head;
        lst->head = node->next;
    }

    return node;
}


int list_put_without_lock(LIST *lst , ElemType value)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    if(NULL == node)
        return FAILED;

    node->value = value;
    node->next = NULL;

    add_to_list_tail(lst , node);

    pthread_mutex_lock(&(lst->lmutex));
    lst->put_ops ++;
    lst->elems ++;
    pthread_mutex_unlock(&(lst->lmutex));

    return SUCCESS;
}

int list_put_with_lock(LIST *lst , ElemType value)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    if(NULL == node)
        return FAILED;

    node->value = value;
    node->next = NULL;

#ifdef DOUBLE_LOCK
    add_to_list_tail_with_double_lock(lst , node);
    pthread_mutex_lock(&(lst->lmutex));
    lst->put_ops ++;
    lst->elems ++;
    pthread_mutex_unlock(&(lst->lmutex));

#else 

    pthread_mutex_lock(&(lst->lmutex));
    add_to_list_tail(lst , node);
    lst->put_ops ++;
    lst->elems ++;
    pthread_mutex_unlock(&(lst->lmutex));

    pthread_cond_signal(&(lst->lcond));
#endif

    return SUCCESS;
}

int list_get_without_lock(LIST *lst , ElemType *val)
{
    NODE *node = del_from_list_head(lst);
    if(NULL == node)
        return FAILED;

    *val = node->value;
    free(node);

    pthread_mutex_lock(&(lst->lmutex));
    lst->get_ops ++;
    lst->elems --;
    pthread_mutex_unlock(&(lst->lmutex));

    return SUCCESS;
}

static int list_get_with_lock_inside(LIST *lst , ElemType *val , int flag)
{
#ifdef DOUBLE_LOCK
    NODE *node = del_from_list_head_with_double_lock(lst);
    if(NULL == node)
    {
        return NULL;
    }

    pthread_mutex_lock(&(lst->lmutex));
    lst->get_ops ++;
    lst->elems --;
    pthread_mutex_unlock(&(lst->lmutex));

#else
    pthread_mutex_lock(&(lst->lmutex));
    NODE *node = del_from_list_head(lst);
    while(NULL == node)
    {
        if(!flag)
        {
            pthread_mutex_unlock(&(lst->lmutex));
            return FAILED;
        }
        pthread_cond_wait(&(lst->lcond) , &(lst->lmutex));
        node = del_from_list_head(lst);
    }

    lst->get_ops ++;
    lst->elems --;
    pthread_mutex_unlock(&(lst->lmutex));
#endif

    *val = node->value;
    free(node);

    return SUCCESS;
}

int list_get_with_lock(LIST *lst , ElemType *val)
{
    return list_get_with_lock_inside(lst , val , 1);
}

int list_get_with_lock_nonblock(LIST *lst , ElemType *val)
{
    return list_get_with_lock_inside(lst , val , 0);
}

int list_size_without_lock(LIST *lst)
{
    int counter = 0;
    NODE *tmp = lst->head;
    while(tmp != NULL)
    {
        tmp = tmp->next;

        ++counter;
    }

    return counter;
}

int list_size_with_lock(LIST *lst)
{
    int counter = 0;
    pthread_mutex_lock(&(lst->lmutex));
    NODE *tmp = lst->head;
    while(tmp != NULL)
    {
        tmp = tmp->next;
        ++counter;
    }
    pthread_mutex_unlock(&(lst->lmutex));

    return counter;
}

void list_state(LIST *lst)
{
    pthread_mutex_lock(&(lst->lmutex));
    printf("Statics : \n");
    printf("Put operation counter : %d , " , lst->put_ops);
    printf("Get operation counter : %d , " , lst->get_ops);
    printf("sum elements counter : %d\n" , lst->elems);
    pthread_mutex_unlock(&(lst->lmutex));
    printf("Elements in the list : %d\n" , list_size(lst));
}

void list_destory(LIST **lst)
{
    if((*lst)->elems != 0)
    {
        printf("----------destory list : -----------\n");
        list_state(*lst);
    }

    NODE *tmp = (*lst)->head;
    while(tmp != NULL)
    {
        NODE  *next = tmp->next;
        free(tmp);
        tmp = next;
    }

    free(*lst);
    *lst = NULL;
}

