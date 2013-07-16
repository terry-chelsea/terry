/*
 * =====================================================================================
 *
 *       Filename:  normal_list.h
 *
 *    Description:  interface of a queue implemented with list...
 *
 *        Version:  1.0
 *        Created:  07/10/13 22:58:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_NORMAL_LIST_H_
#define _H_NORMAL_LIST_H_

#include <pthread.h>
#include <stdio.h>

typedef void * ElemType;
typedef struct node NODE;
struct node
{
    ElemType value;
    NODE *next;
};

typedef struct list
{
    NODE *head;
    NODE *tail;
    int  put_ops;
    int  get_ops;
    int  elems;
    //the only one mutex to protect and int elements and head and tail 
    //when define ATOMIC...
    pthread_mutex_t lmutex;
    pthread_cond_t  lcond;

    //use double lock to protect head and tail...
    //DOUBLE_LOCK must define after define ATOMIC...
    //However , I did not find any advantage compare to signal mutex...
#ifdef DOUBLE_LOCK
    pthread_mutex_t tail_mutex;
    pthread_mutex_t head_mutex;
#endif
}LIST;

#define SUCCESS 0
#define FAILED  -1

LIST *list_create();

int list_put_without_lock(LIST *lst , ElemType value);

int list_put_with_lock(LIST *lst , ElemType value);

int list_get_without_lock(LIST *lst , ElemType *val);

int list_get_with_lock(LIST *lst , ElemType *val);

int list_get_with_lock_nonblock(LIST *lst , ElemType *val);

int list_size_with_lock(LIST *lst);

int list_size_without_lock(LIST *lst);

void list_state(LIST *lst);

void list_destory(LIST **lst);


#ifdef ATOMIC

#define list_put(lst , value) list_put_with_lock(lst , value)
#define list_get(lst , value) list_get_with_lock(lst , value)
#define list_get_nonblock(lst , value) list_get_with_lock_nonblock(lst , value)
#define list_size(lst)        list_size_with_lock(lst)

#else

#define list_put(lst , value) list_put_without_lock(lst , value)
#define list_get(lst , value) list_get_without_lock(lst , value)
#define list_size(lst)        list_size_without_lock(lst)

#endif

#endif

