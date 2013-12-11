/*
 * =====================================================================================
 *
 *       Filename:  Disk.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:08:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Disk.h"
#include "Communication.h"
#include "Socket.h"

int deal_with_check_disk(int fd)
{
    int expired_times = 0;
    if(read_timer(fd , &expired_times) < 0)
    {
        LOG_ERROR("In deal_with_check_disk : read timer fd error : %s" , STR_ERROR);
        return -1;
    }         
    LOG_INFO_TIME("In deal_with_check_disk : read heartbeat timer expired %d times" , expired_times);

    if(expired_times <= 0)
    {
        LOG_WARNING("In deal_with_check_disk : read timer fd is 0!");
        return 0;
    }

    g_global_value.basic_timer_counter += expired_times;

    int ret = -1;
    int notice_thread_flag = 0;
    
    if(g_global_value.basic_timer_counter % xml_config.check_disk_interval == 0)                       //需要检查磁盘
    {
        ret = check_disk_state();
        if(ret < 0)
        {
            LOG_ERROR("In deal_with_check_disk : check disk state error !");
            return -1;
        }
        if(1 == ret && 0 == g_global_value.thread_blocking)                             //需要暂停并且当前没有暂停
        {
            if(notice_thread_blocking() < 0)
            {
                LOG_ERROR("In deal_with_check_disk : notice thread block error !");
                return -1;
            }
            notice_thread_flag = 1;
        }
        if(0 == ret && 1 == g_global_value.thread_blocking)                           //需要唤醒并且当前线程已暂停
        {
            if(notice_thread_working() < 0)
            {
                LOG_ERROR("In deal_with_check_disk : notice thread work error !");
                return -1;
            }
            notice_thread_flag = 1;
        }

        NODE *append_list = g_global_value.append_bucket_list;
        if((0 == notice_thread_flag) &&  !list_is_empty(append_list))                    //每次只能通知阻塞、运行，或者通知新桶号
        {
            NODE *front = list_remove_front_node(append_list);                           //前面已经保证链表不为空，所以始终返回正确
            if(front != NULL)
            {
                int bucket_nr = front->value;
                if(notice_thread_new_bucket(bucket_nr) < 0)
                {
                    LOG_ERROR("In deal_with_check_disk : append new bucket No.%d to thread error !" , bucket_nr);
                    list_preappend_node(append_list , front);                           //出错重新添加到链表上
                }
                else
                    free(front);                                                        //没出错释放这个节点
            }
        }

    }

    NODE *recovery_list = g_global_value.recovery_bucket_list;
    int bucket_nr = -1;
    if(list_is_empty(recovery_list))
        return 0;

    if(g_global_value.basic_timer_counter % CHECK_RECOVER_INTERVAL == 0)
    {
        ret = recheck_bucket_state();
        if(ret < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : recheck bucket state error !");
        }
    } 

    return 0;
}

//这里只是一个简单的模拟代码
int check_disk_state()
{
    srand(time(NULL));
    static int stamp = 0;
    static int in_blocking = 0;

    int random = rand() % 1000 ;
    if(random % 371 == 0)                                            //这种情况下返回需要暂停.
    {
        in_blocking = 0;
        stamp = 0; 
        return 1;
    }
    else if(0 == in_blocking && stamp < 30)                          //暂停60秒
        return 1;

    return 0;                                                        //其他情况，每次总是返回0。
}

//重新查看是否有处于恢复状态的桶 已经恢复完成.
int recheck_bucket_state()
{
    int bucket_nr = -1;
    NODE *prev = g_global_value.recovery_bucket_list;
    NODE *node = prev->next;
    NODE *temp = NULL;

    while(node != NULL)
    {
        temp = node;
        bucket_nr = node->value;

        if(check_bucket_recovery(bucket_nr) == 0)                             //一个已经恢复完成的桶
        {
            NODE *append_list = g_global_value.append_bucket_list;
            if(NULL == append_list)
            {
                append_list = list_create();
                if(NULL == append_list)
                {
                    LOG_ERROR("In recheck_bucket_state : create append list error !");
                    return -1;
                }
                g_global_value.append_bucket_list = append_list;
            }
            prev->next = node->next;                                         //在恢复链表上删除原节点
            prev = prev->next;                                               //prev移到原链表的下一个元素
            node = prev->next;                                               //node也移到下一个元素
            list_preappend_node(append_list , temp);                         //将删除的节点添加到append_list上
        }
    }
    
    return 0;
}

