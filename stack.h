/*
 * =====================================================================================
 *
 *       Filename:  stack.h
 *
 *    Description:  just a stack using array...
 *
 *        Version:  1.0
 *        Created:  05/21/13 18:17:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_COMMON_STACK_H_
#define _H_COMMON_STACK_H_

typedef int ELEM_TYPE;

#define O1_MAX_MIN_

typedef struct 
{
    ELEM_TYPE value;
#ifdef O1_MAX_MIN_
    ELEM_TYPE min;
    ELEM_TYPE max;
#endif
}Elem;

//每次扩展一倍的空间
typedef struct Stack
{
    Elem* all_elements;
    unsigned int size;
    unsigned counter;
}Stack;


Stack *create_stack(ELEM_TYPE * , int);

void stack_push(Stack * , ELEM_TYPE);

ELEM_TYPE stack_pop(Stack *);

ELEM_TYPE stack_top(Stack *);

int  stack_empty(Stack *);

void stack_clear(Stack *);

ELEM_TYPE stack_max(Stack *);

ELEM_TYPE stack_min(Stack *);

void stack_reverse(Stack *);

void destory_stack(Stack *);

#endif
