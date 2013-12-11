/*
 * =====================================================================================
 *
 *       Filename:  Communication.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:50:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_COMMUNICATION_H_
#define _H_TERRY_COMMUNICATION_H_

#include "Header.h"

extern int notice_thread_blocking();

extern int create_pipes(int epoll_fd);

extern void close_pipe(int *pipe_fds);

extern int notice_thread_working();

extern int notice_thread_new_bucket(int bucket_nr);

extern int notice_thread_start_recycle();

extern int deal_with_thread_notice(int fd);

extern int thread_check_waiting(int *read_value);

extern int thread_wait_main_wakeup();

#endif

