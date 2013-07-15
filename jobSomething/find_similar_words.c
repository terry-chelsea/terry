/*
 * =====================================================================================
 *
 *       Filename:  find_similar_words.c
 *
 *    Description:  find the same combination...
 *
 *        Version:  1.0
 *        Created:  07/08/13 10:23:07
 *       Revision:  none
 *       Compiler:  gcc
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN  256

typedef struct elem
{
    char *strs;
    struct elem * next;
}Element;

typedef struct join
{
    char *strs;
    struct join *next;
    Element *subs;
}Join;

static Join *sorted = NULL;

int compar(const void * one , const void * ano)
{
    return (*(const char *)one - *(const char *)ano);
}

void insert_to_list(Element **head , Element *elem)
{
    if(NULL == *head)
    {
        *head = elem;
        elem->next = NULL;
    }
    else 
    {
        elem->next = *head;
        *head = elem;
    }
}

void insert_to_sorted(char *cp , int len , Element *elem)
{
    int flags = 0;
    char *str = NULL;
    if(NULL == sorted)
    {
        goto INSERT;
        return ;
    }

    int ret = strcmp(sorted->strs , cp);
    if(!ret)
    {
        insert_to_list(&(sorted->subs) , elem);
        return ;
    }
    else if(ret > 0)
    {
        flags = 1;
        goto INSERT;
    }

    Join *tmp = sorted->next;
    Join *prev = sorted;
    while(tmp != NULL)
    {
        int cmp = strcmp(tmp->strs , cp);
        if(!cmp)
        {
            insert_to_list(&(tmp->subs) , elem);
            return ;
        }
        
        if(cmp > 0)
            break;
        prev = tmp;
        tmp = prev->next;
    }

INSERT :
    str = (char *)malloc(len);
    memcpy(str , cp , len);

    Join *new_join = (Join *)malloc(sizeof(Join));
    memset(new_join , 0 , sizeof(Join));
    new_join->strs = str;
    new_join->next = NULL;
    new_join->subs = elem;
    elem->next = NULL;

    if(NULL == sorted)
    {
        sorted = new_join;
    }
    else 
    {
        if(flags)
        {
            new_join->next = sorted;
            sorted = new_join;
        }
        else 
        {
            new_join->next = prev->next;
            prev->next = new_join;
        }
    }
}

void get_a_element(char *elem , char *cp , int len)
{
    Element *new_one = (Element *)malloc(sizeof(Element));
    memset(new_one , 0 , sizeof(*new_one));
    new_one->strs = elem;
    new_one->next = NULL;
    
    qsort(cp , len - 1 , sizeof(char) , compar);
    insert_to_sorted(cp , len , new_one);
}

void output_clear_list(Join *lst)
{
    Element *head = lst->subs;
    Element *next = NULL;
    while(head != NULL)
    {
        next = head->next;
        char *str = head->strs;
        fprintf(stdout , "%s  ", str);
        free(str);
        free(head);
        head = next;
    }
    printf("\n");
    free(lst->strs);
    free(lst);
}

void output_result_and_clean()
{
    Join *start = sorted;
    Join *next = NULL;
    while(start != NULL)
    {
        next = start->next;
        output_clear_list(start);
        start = next;
    }
}

#ifdef _TEST_
void out_all()
{
    Join *pass = sorted;
    while(pass != NULL)
    {
        printf("%s : \n" , pass->strs);
        Element *head = pass->subs;
        while(head != NULL)
        {
            printf("%s  " , head->strs);
            head = head->next;
        }
        printf("\n");
        pass = pass->next;
    }
}
#else 
void out_all()
{}

#endif

int main()
{
    char input[MAX_LEN]; 
    while(1)
    {
        if(fgets(input , MAX_LEN , stdin) == NULL)
            break;

        int len = strlen(input);
        if(input[len - 1] == '\n')
            input[len - 1] = '\0';

        char *elem = (char *)malloc(len);
        memcpy(elem , input , len);

        get_a_element(elem , input , len);

        out_all();
    }

    output_result_and_clean();

    return 0;
}
