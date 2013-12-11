/*
 * =====================================================================================
 *
 *       Filename:  timer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:05:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_TIMER_H_
#define _H_TERRY_TIMER_H_

#include<sys/timerfd.h>
#include<sys/time.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

extern int create_timer(int interval_value , int epoll_fd);

extern int read_timer(int fd , int *times);

#endif
