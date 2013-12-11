/*
 * =====================================================================================
 *
 *       Filename:  Socket.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 05:27:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef _TERRY_SOCKET_H_
#define _TERRY_SOCKET_H_

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/timerfd.h>
#include<sys/time.h>
#include<time.h>
#include "Header.h"

typedef struct in_buf
{
    MsgHeader header;                      //报文的头部
    char *io_buf;                           //负载数据
    int  length;                           //负载数据的长度
}In_Req;

typedef struct out_buf
{
    char *req_msg;                        //需要发送的缓冲区，包括报文头和负载
    int  length;                          //需要发送数据的长度
    struct out_buf *next;                 //为了发送链表
}Out_Buf;

typedef struct Agent 
{
    Out_Buf *send_buf;                      //数据总是能够写出去，使用非阻塞方式.

    In_Req  recv_buf;                       //接收缓冲区，一次最多只接受一个完整的报文。
    int init_flag;                          //是否是一个新的报文
    int read_all_header_flag;               //是否已经将一个报文的报文头全部读取
    int read_header_length;                 //报文头已经读取的字节数，需要读取的字节数是sizeof(MsgHeader).
    int read_all_data_flag;                 //是否已经将报文的负载数据全部读取
    int read_data_length;                   //实际已经读取的负载数据大小，需要读取的字节数是recv_buf.header.length.

    time_t last_heartbeat_timestamp;        //上次接收到心跳的时间戳
}Agent;

extern  Agent *heartbeat_agent;

extern int setAddress(char *ip , char port , struct sockaddr_in *addr);

extern int create_listen_socket_add_epoll(char *ip , short port , int epoll_fd);

extern int deal_with_new_connect(int fd , int epoll_fd);

extern void clear_agent();

extern int read_header(int fd , int from_index , int expect_length , int *is_finish);

extern int read_data(int fd , int from_index , int expect_length , int *is_finish);

extern void close_this_connect();

extern int check_bucket_recovery(int bucket_nr);

#endif
