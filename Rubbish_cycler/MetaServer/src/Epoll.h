/*
 * =====================================================================================
 *
 *       Filename:  Epoll.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 05:15:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _TERRY_EPOLL_H_
#define _TERRY_EPOLL_H_

#include<sys/epoll.h>
#include "Header.h"

#define EPOLL_NUM   32
#define TIME_OUT    5000

extern int create_epoll(int maxEpoll);

extern int add_to_epoll(int epollFd , int fd , int events , void *ptr);

extern int modify_in_epoll(int epollFd , int fd , int events);

extern int delete_from_epoll(int epollFd , int fd);

extern int epoll_run(int epoll_fd);

static void do_something_else(int socket_error , int pipe_error);

#endif
