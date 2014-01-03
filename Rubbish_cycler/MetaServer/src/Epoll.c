/*
 * =====================================================================================
 *
 *       Filename:  Epoll.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 05:16:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Epoll.h"
#include "Socket.h"
#include "Disk.h"
#include "Heartbeat.h"
#include<string.h>
#include<errno.h>


extern GLOBLE_VALUE g_global_value;

int create_epoll(int maxEpoll)
{
    int fd = -1;
    fd = epoll_create(maxEpoll);

    if(fd < 0)
    {
        LOG_ERROR("create epoll error : %s" , STR_ERROR);
        return -1;
    }

    return fd;
}

int add_to_epoll(int epollFd , int fd , int events , void *ptr)
{
    struct epoll_event event;
    memset(&event , 0 , sizeof(event));

    event.events = events;
    if(NULL == ptr)
        event.data.fd = fd;
    else
        event.data,ptr = ptr;

    if(epoll_ctl(epollFd , EPOLL_CTL_ADD , fd , &event) < 0)
    {
        LOG_ERROR("epoll_ctl add fd %d error : %s" , fd , STR_ERROR);
        return -1;
    }
    return 0;
}

int modify_in_epoll(int epollFd , int fd , int events)
{
    struct epoll_event event;

    event.events = events;
    if(epoll_ctl(epollFd , EPOLL_CTL_MOD , fd , &event) < 0)
    {
        LOG_ERROR("epoll_ctl modify fd %d error : %s" , fd , STR_ERROR);
        return -1;
    }
    return 0;
}

int delete_from_epoll(int epollFd , int fd)
{
    if(epoll_ctl(epollFd , EPOLL_CTL_DEL , fd , NULL) < 0)
    {
        LOG_ERROR("epoll_ctl delete fd %d error : %s" , fd , STR_ERROR);
        return -1;
    }
    return 0;
}

int epoll_run(int epoll_fd)
{
    struct epoll_event ready_events[EPOLL_NUM];
    memset(ready_events , 0 , sizeof(struct epoll_event) * EPOLL_NUM);

    int socket_error = 0;
    int pipe_error = 0;
    while(1)
    {
        int nreadys = epoll_wait(epoll_fd , ready_events , EPOLL_NUM , TIME_OUT);
        if(nreadys < 0)
        {
            if(EINTR == errno)
            {
                continue ;
            }
            else
            {
                LOG_ERROR("epoll_wait in fd %d error : %s" , epoll_fd , STR_ERROR);
                return -1;
            }

        }

        int i = 0;
        for(i = 0 ; i < nreadys ; ++ i)
        {
            int ready_fd = ready_events[i].data.fd;
            if(ready_fd < 0)
            {
                LOG_WARNING("In epoll_run : fd %d is ready in epoll" , ready_fd);
                continue ;
            }

            int ret = -1;
            if(ready_fd == g_global_value.g_listen_fd)
            {
                if(deal_with_new_connect(ready_fd , epoll_fd) < 0)
                {
                    LOG_ERROR("In epoll_run : deal_with_new_connect error !");
                    socket_error = 1;
                }
            }
            else if(ready_fd == g_global_value.heartbeat_ack_fd)
            {
                //所以的错误事件都会触发EPOLLIN和EPOLLOUT，不单独检查错误。
                unsigned int event = ready_events[i].events;
                if(event & EPOLLIN)
                {
                    ret = deal_with_socket_in(ready_fd , epoll_fd);
                    if(ret < 0)
                    {
                        LOG_ERROR("In epoll_run : deal with socket in error !!!");
                        socket_error = 1;
                    }
                    else if(ret)
                    {
                        LOG_WARNING_TIME("In epoll_run : CS close this socket !!!");
                        socket_error = 1;
                    }
               }
               if(event & EPOLLOUT)
               {
                    ret = deal_with_socket_out(ready_fd , epoll_fd);
                    if(ret < 0)
                    {
                        LOG_ERROR("In epoll_run : deal with socket out error !!!");
                        socket_error = 1;
                    }
               }

            }
            else if(ready_fd ==( g_global_value.thread_to_main)[0])
            {
                ret =  deal_with_thread_notice(ready_fd);
                if(ret < 0)
                {
                    LOG_ERROR("In epoll_run : deal with thread notice error !!!");
                    pipe_error = 1;
                }
                else if(ret)
                {
                    LOG_WARNING_TIME("In epoll_run : detect thread close pipe , NOT ALLOWED !!!");
                    pipe_error = 1;
                }

            }
            else if(ready_fd ==  g_global_value.check_disk_fd)
            {
                if(deal_with_check_disk(ready_fd) < 0)
                {
                    LOG_ERROR("In epoll_run : deal with check disk error !!!");
                    pipe_error = 1;
                }
                
            }
            else if(ready_fd == g_global_value.send_heartbeat_fd)
            {
                if(deal_with_send_heartbeat(ready_fd , g_global_value.heartbeat_ack_fd , epoll_fd) < 0)
                {
                    LOG_ERROR("In epoll_run : deal_with_send_heartbeat error !");
                    socket_error = 1;
                }
            }
            else 
            {
                LOG_WARNING("Undefined return fd : %d" , ready_fd);
                break ;
            }
        }

        do_something_else(socket_error , pipe_error);
    }

    return 0;
}

void do_something_else(int socket_error , int pipe_error)
{
    if(socket_error)
    {
        close_this_connect();
    }
    if(pipe_error)
    {
        exit(-1);
    }
}
