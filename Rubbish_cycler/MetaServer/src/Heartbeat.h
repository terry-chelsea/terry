/*
 * =====================================================================================
 *
 *       Filename:  Heartbeat.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:06:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_HEART_BEAT_H_
#define _H_TERRY_HEART_BEAT_H_

#include "Header.h"
#include "Socket.h"

extern int deal_with_send_heartbeat(int fd , int socket_fd , int epoll_fd);

static int check_CS_timeout();

static void update_timestamp();

static int send_all_finish_bucket();

static int add_to_send_buffer(MsgHeader head , void *data , int length);

static void add_one_to_send_buffer(Out_Buf *buf);

extern int deal_with_socket_in(int socket_fd , int epoll_fd);

static  void close_heartbeat_socket(int socket_fd , int epoll_fd);

static int read_back(In_Req req , int epoll_fd);

static int MU_start_rubbish_recycle(In_Req req , int epoll_fd);

static int add_new_bucket(int bucket_nr , int flag);

static int start_rubbish_recycle(int epoll_fd);

static int MU_add_new_bucket(In_Req req);

static int MU_CS_heartbeat_ack(In_Req req);

extern int deal_with_socket_out(int socket_fd , int epoll_fd);

static int send_heartbeat(int fd);

#endif
