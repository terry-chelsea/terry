
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "stack.h"

#define INIT_SIZE 16

Stack *create_stack(ELEM_TYPE * init , int size)
{
    if(init != NULL && size <= 0)
    {
        fprintf(stderr , "Invalid value !");
        return NULL;
    }

    Stack *st = (Stack *)malloc(sizeof(Stack));
    if(NULL == st)
    {
        fprintf(stderr , "Allocate memory for stack failed !");
        return NULL;
    }
    memset(st , 0 , sizeof(Stack));
    
    int is_cp = 0;
    if(NULL == init || size <= 0)
    {
        size = INIT_SIZE;
        is_cp = 0;
    }
    else 
    {
        is_cp = 1;
    }

    Elem *elems = (Elem *)malloc(sizeof(Elem) * size);
    if(NULL == elems)
    {
        fprintf(stderr , "Allocate memory for array failed !");
        goto FREE1;
    }
    memset(elems , 0 , sizeof(Elem) * size);

    st->all_elements = elems;
    st->size = size;
    if(is_cp)
    {
        int i = 0;
        for(i = 0 ; i < size ; ++ i)
            stack_push(st , init[i]);
    }
    else 
    {
        st->counter = 0;
    }

    return st;
    
FREE1 :
    free(st);
    st = NULL;
    return NULL;
}

#define ALIGN_TO(sz) ((sz + 3) & ~3)

int append_stack(Stack *st)
{
    int new_size = (st->size * 3) / 2;
    new_size = ALIGN_TO(new_size) + 4;

    Elem *new_elems = (Elem *)realloc(st->all_elements , sizeof(Elem) * new_size);
    if(NULL == new_elems)
    {
        fprintf(stderr , "Append new memory for stack failed !");
        return -1;
    }
    
    st->all_elements = new_elems;
    st->size = new_size;
    return 0;
}

void stack_push(Stack * st, ELEM_TYPE value)
{
    if(NULL == st)  return ;
    if(st->counter == st->size)
        if(append_stack(st) < 0)
            return ;
    
    (st->all_elements)[st->counter].value = value;

#ifdef O1_MAX_MIN_
    if(stack_empty(st))
    {
        (st->all_elements)[st->counter].min = value;
        (st->all_elements)[st->counter].max = value;
    }
    else
    {
        ELEM_TYPE last_min = (st->all_elements)[st->counter - 1].min;
        ELEM_TYPE last_max = (st->all_elements)[st->counter - 1].max;
        (st->all_elements)[st->counter].min = (value < last_min ? value : last_min);
        (st->all_elements)[st->counter].max = (value > last_max ? value : last_max);
    }
#endif 
    
    st->counter ++;
}

ELEM_TYPE stack_pop(Stack *st)
{
    if(NULL == st)
        return (ELEM_TYPE)0;

    ELEM_TYPE ret = (st->all_elements)[st->counter - 1].value;
    st->counter --;

    return ret;
}

ELEM_TYPE stack_top(Stack *st)
{
    if(NULL == st)
        return (ELEM_TYPE)0;

    return (st->all_elements)[st->counter - 1].value;
}

int stack_empty(Stack *st)
{
    return (NULL == st) || (0 == st->counter);
}

void stack_clear(Stack *st)
{
    if(NULL == st)
        return ;

    st->counter = 0;
}

ELEM_TYPE stack_min(Stack *st)
{
    if(NULL == st || st->counter == 0)
        return (ELEM_TYPE)0;

#ifdef O1_MAX_MIN_
    return (st->all_elements)[st->counter - 1].min;
#endif

    int i = 0;
    Elem *all = st->all_elements;
    ELEM_TYPE min = all[0].value;
    for(i = 0 ; i < st->counter ; ++ i)
    {
        if(min > all[i].value)
            min =  all[i].value;
    }

    return min;
}

ELEM_TYPE stack_max(Stack *st)
{
    if(NULL == st || st->counter == 0)
        return (ELEM_TYPE)0;

#ifdef O1_MAX_MIN_
    return (st->all_elements)[st->counter - 1].max;
#endif

    int i = 0;
    Elem *all = st->all_elements;
    ELEM_TYPE max = all[0].value;
    for(i = 0 ; i < st->counter ; ++ i)
    {
        if(max < all[i].value)
            max =  all[i].value;
    }

    return max;
}

void add_elem_to_stack(Stack * st , ELEM_TYPE val)
{
    if(stack_empty(st))
        stack_push(st , val);
    else 
    {
        ELEM_TYPE top = stack_pop(st);
        add_elem_to_stack(st , val);

        stack_push(st , top);
    }

}

void stack_reverse(Stack *st)
{
    if(NULL == st || st->counter == 0)
        return ;

    if(!stack_empty(st))    
    {
        ELEM_TYPE top = stack_pop(st);
        stack_reverse(st);

        add_elem_to_stack(st , top);
    }
}

void destory_stack(Stack *st)
{
    if(NULL == st)
        return ;

    Elem *all = st->all_elements;
    if(all != NULL)
        free(all);

    free(st);
    st = NULL;
}
