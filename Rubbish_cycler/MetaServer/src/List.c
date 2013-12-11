/*
 * =====================================================================================
 *
 *       Filename:  List.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 18:21:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "List.h"
#include "Log/Debug.h"

NODE *list_create()
{
    NODE *head = (NODE *)malloc(sizeof(NODE));
    if(NULL == head)
    {
        LOG_ERROR("In list_create : malloc list head error!");
        return NULL;
    }

    head->value = 0;
    head->next = NULL;

    return head;
}

void list_move(NODE *head_src , NODE *head_dest)
{
    NODE *node_dest = head_dest->next;
    //释放目的链表上的所有元素
    while(node_dest != NULL)
    {
        NODE *temp = node_dest;
        node_dest = temp->next;
        free(temp);
    }
    head_dest->next = NULL;
    head_dest->value = 0; 

    head_dest->next = head_src->next;
    head_dest->value = head_src->value;

    head_src->next = NULL;
    head_src->value = 0;
}

int list_preappend_value(NODE *head , int new_value)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    if(NULL == node)
    {
        LOG_ERROR("In list_append_value : malloc new node error!");
        return -1;
    }
    node->value = new_value;
    preappend_one_node(head , node);

    ++ (head->value);
    return 0;
}

int list_is_empty(NODE *head)
{
    return (head->next == NULL);
}

void list_preappend_node(NODE *head , NODE *node)
{
    node->next = head->next;
    head->next = node;

    ++ head->value;
}

NODE *list_remove_front_node(NODE *head)
{
    NODE *node = head->next;
    if(NULL == node)                                    //链表为空
        return NULL;

    head->next = node->next;
    node->next = NULL;
    -- head->value;

    return node;
}

/*保证它是一致性的操作 */
int list_append_array(NODE *head , int *array_value , int array_size)
{
    NODE *temp_head = NULL;
    NODE *temp_node = NULL;
    int i = 0;
    for(i = 0 ; i < array_size ; ++ i)
    {
        NODE *node = (NODE *)malloc(sizeof(NODE));
        if(NULL == node)
        {
            while((temp_node = temp_head) != NULL)
            {
                temp_head = temp_node->next;
                free(temp_node);
            }
            LOG_ERROR("In list_append_array : malloc one node error!");
            return -1;
        }
        node->value = array_value[i];
        if(NULL == temp_head)
        {
            temp_head = node;
            temp_node = node;
        }
        else
        {
            node->next = temp_node->next;
            temp_node->next = node;
        }
    }
    temp_node = head;
    while(temp_node != NULL)
        temp_node = temp_node->next;

    temp_node->next = temp_head;
    head->value += array_size;

    return 0;
}

int list_size(NODE *head)
{
    return head->value;
}

int list_get_and_delete_front_node(NODE *head , int *ret_value)
{
    NODE *node = head->next;
    if(NULL == node)
    {
        *ret_value = 0;
        return -1;
    }
    else
    {
        head->next = node->next;
        *ret_value = node->value;
        free(node);
        -- head->value;
        return 0;
    }
}

void list_destory(NODE *head)
{
    NODE *node = head->next;
    NODE *temp_node = NULL;
    while(node != NULL)
    {
        temp_node = node->next;
        free(node);
        node = temp_node;
    }
}
