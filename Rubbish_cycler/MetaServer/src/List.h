/*
 * =====================================================================================
 *
 *       Filename:  List.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 18:14:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _TERRY_LIST_H_
#define _TERRY_LIST_H_

#include<stdlib.h>

#define FOR_EACH_ENTRY(head , value) \
    for(Node *node = head->next ; (node != NULL) && ((value = node->value) |１）; node = node->next)

typedef struct node
{
    int value;
    struct node *next;
}NODE;

extern NODE* list_create();

extern int list_preappend_value(NODE *head , int new_value);

extern void list_preappend_node(NODE *head , NODE *node);

extern NODE *list_remove_front_node(NODE *head);

extern void list_move(NODE *head_src , NODE *head_dest);

extern int list_is_empty(NODE *head);

extern int list_append_array(NODE *head , int *array_value , int array_size);

extern int list_size(NODE *head);

extern int list_get_and_delete_front_node(NODE * head, int *ret_value);

extern void list_destory(NODE *head);

#endif

