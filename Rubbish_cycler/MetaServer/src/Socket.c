/*
 * =====================================================================================
 *
 *       Filename:  Socket.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 05:31:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Socket.h"
#include "Epoll.h"
#include<stdint.h>

#define STR_ERROR  strerror(errno)
#define LISTENQ  6

//套接字的读写使用阻塞的方式，这是因为在当前的系统设计中主线程可以引起阻塞。
//可能引起阻塞的操作有以下几种：
//接受连接accept，在连接到来的时候：deal_with_new_connect
//读套接字read，在读取CS的心跳信息的时候：read_with_blocking
//写套接字write，在心跳套接字写事件被触发的时候：write_with_blocking
//检查磁盘的是否繁忙，这个需要查看磁盘，所以可能会引起阻塞。
//向服务模块查看桶状态，通过网络套接字接口，非阻塞读写
//
//但是MU的主线程的主要作用是保持心跳连接，但是心跳会有一个最大时延
//但是定时器即使在阻塞的时候也会在进程重新执行之后被激活，所以仍然会发送心跳
//另外，读写时间是epoll触发的，并且只读一次，所以可能没有读到期望的数据，但是不会阻塞。
//write操作可能会阻塞，但是心跳信息每次只有一个报文头，所以发送缓冲区写满基本上是不可能的。
//假设心跳2秒每次，超时时延为2分钟，所以因为阻塞事件超时是不可能的，即使发生了，因为一个事件阻塞2分钟，那这个节点也是不可用的。
//呼呼，毕竟垃圾回收不是对性能有太严格的要求，它只是一个功能性的系统。

Agent *heartbeat_agent = NULL;

int setAddress(char *ip , char port , struct sockaddr_in *addr)
{
    memset(addr , 0 , sizeof(*addr));
    addr->sin_family = AF_INET;
    if(NULL == ip)
    {
        addr->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        int err = inet_pton(AF_INET , ip , &(addr->sin_addr));
        if(err < 0)
        {
            LOG_ERROR("inet_pton error : %s" , STR_ERROR);
            return -1;
        }
    }

    addr->sin_port = htons(port);

    return 0;
}


int create_listen_socket_add_epoll(char *ip , short port , int epoll_fd)
{
    struct sockaddr_in addr;

    int fd = -1;
    const int on = 1;

    setAddress(ip , port , &addr);

    fd = socket(AF_INET , SOCK_STREAM , 0);
    if(fd < 0)
    {
        LOG_ERROR("Create Listen Socket error : %s" , STR_ERROR);
        return -1;
    }

    if(setsockopt(fd , SOL_SOCKET , SO_REUSEADDR , (char *)&on , sizeof(on)) < 0)
    {
        LOG_ERROR("setsockopt in fd %d Enable Reuseaddr error : %s" , fd , STR_ERROR);
        close(fd);
        return -1;
    }

    if(bind(fd , (struct sockaddr *)&addr , sizeof(addr)) < 0)
    {
        LOG_ERROR("bind fd %s to addr error : %s" , fd , STR_ERROR);
        close(fd);
        return -1;
    }

    if(listen(fd , LISTENQ) < 0)
    {
        LOG_ERROR("listen on fd %s error : %s" , fd , STR_ERROR);
        close(fd);
        return -1;
    }

    if(add_to_epoll(epoll_fd , fd , EPOLLIN , NULL) < 0)
    {
        LOG_ERROR("add fd %d to epoll error!" , fd);
        close(fd);
        return -1;
    }

    return fd;
}

int deal_with_new_connect(int fd , int epoll_fd)
{
    struct sockaddr_in client_addr;
    memset(&client_addr , 0 , sizeof(client_addr));
    socklen_t addr_len = sizeof(client_addr);

    int connfd = accept(fd , (struct sockaddr *)&client_addr , &addr_len);
    if(connfd < 0)
    {
        LOG_ERROR("In deal_with_new_connect : accept error : %s" , STR_ERROR);
        return -1;
    }

    if(g_global_value.in_connecting)
    {
        close(connfd);
        LOG_WARNING("In deal_with_new_connect : connect request when connecting , close the socket!");
        return 0;
    }

    NODE *new_list = list_create();
    if(NULL == new_list)
    {
        LOG_ERROR("In deal_with_new_connect : create finish list error!");
        close(connfd);
        return -1;
    }
    g_global_value.finish_bucket_list = new_list;

    /*  
    if(setNonblock(connfd) < 0)
    {
        LOG_ERROR("In deal_with_new_connect : set new socket %d to nonblocking error !" , connfd);
        close(connfd);
        return -1;
    }
    */

    if(add_to_epoll(epoll_fd , connfd , EPOLLIN , NULL) < 0)
    {
        LOG_ERROR("In deal_with_new_connect : add socket %d to epoll error !" , connfd);
        close(connfd);
        return -1;
    }

    heartbeat_agent = (Agent *)malloc(sizeof(Agent));
    if(NULL == heartbeat_agent)
    {
        LOG_ERROR("In deal_with_new_connect : allocate memory for Agent error !");
        close(connfd);
        return -1;
    }
    memset(heartbeat_agent , 0 , sizeof(*heartbeat_agent));

    g_global_value.heartbeat_ack_fd = connfd;

    g_global_value.in_connecting = 1;

    return 0;
}

void clear_agent()
{
    if(heartbeat_agent->send_buf != NULL)
    {
        Out_Buf *buf = heartbeat_agent->send_buf;
        Out_Buf *temp = NULL;
        while(buf != NULL)
        {
            temp = buf;
            buf = buf->next;
            free(temp);
        }
        heartbeat_agent->send_buf = NULL;
    }
    if(heartbeat_agent->recv_buf.io_buf != NULL)
    {
        free(heartbeat_agent->recv_buf.io_buf);
        heartbeat_agent->recv_buf.io_buf = NULL;
    }
}

//fd是读取的描述符，from_index是需要读取的报文头的偏移量，expect_length是本次期望读取的最大字节数，is_finish作为返回值表示是否读取结束。
//只有两种适用场景：
//1、需要读取整个的报文头，from_index为0，expect_length为MsgHeader的长度。
//2、需要读取不分的报文头，from_index的值为read_header_length，expect_length为MsgHeader的长度减去from_index
//总之，期望将整个的报文头读取完整。
int read_header(int fd , int from_index , int expect_length , int *is_finish)
{
    int read_ret = -1;
    int read_length = expect_length;
    MsgHeader header;

    read_ret = read_with_blocking(fd , (char *)&header , &read_length);             //读取一次数据，读取的数据保存在header中，读取的数据字节数保存在read_length中
    if(read_ret < 0)
    {
        LOG_ERROR("In read_header : read header error of fd %d !" , fd);
        return -1;
    }
    else if(read_ret)
    {
        LOG_WARNING_TIME("In read_header : opposite close heartbeat socket fd %d!!!" , socket_fd);
        return 1;
    }

    char *msg_header = (char *)(&(heartbeat_agent->recv_buf.header));                        //数据应该保存在这个地方
    memcpy(msg_header + from_index , &header , read_length);                                 //拷贝数据

    heartbeat_agent->read_all_data_flag = 0;
    heartbeat_agent->read_data_length = 0;
    if(read_length == expect_length)                             //如果读取到期望读取的数据，说明已经读取完数据
    {
        heartbeat_agent->read_all_header_flag = 1;               //读取了完整的报文头
        heartbeat_agent->read_header_length = 0;

        *is_finish = 1;
    }
    else                                                        //如果没有读取到期望的数据
    {
        heartbeat_agent->read_all_header_flag = 0;
        heartbeat_agent->read_data_length += read_length;       //将下次读取的偏移量保存
        
        *is_finish = 0;
    }

    return 0;
}

int read_data(int fd , int from_index , int expect_length , int *is_finish)
{
    int read_ret = -1;
    int read_length = expect_length;
    MsgHeader header;

    char *read_buf = heartbeat_agent->recv_buf.io_buf;                       
    if(NULL == read_buf)
    {
        LOG_WARNING("In read_data : heartbeat_agent recv buffer is NULL !!!");
        return -1;
    }
    read_ret = read_with_blocking(fd , read_buf + from_index , &read_length);       // 直接读取到数据缓冲区中。             
    if(read_ret < 0)
    {
        LOG_ERROR("In read_data : read data error of fd %d !" , fd);
        return -1;
    }
    else if(read_ret)
    {
        LOG_WARNING_TIME("In read_data : opposite close heartbeat socket fd %d!!!" , socket_fd);
        return 1;
    }

    if(read_length == expect_length)                                 //一次没有读取结束
    {
        heartbeat_agent->read_all_data_flag = 1;                    //数据全部读取完了
        heartbeat_agent->read_data_length = 0;

        *is_finish = 1;
    }
    else 
    {
        heartbeat_agent->read_all_data_flag = 0;
        heartbeat_agent->read_data_length += read_length;

        *is_finish = 0;
    }

    return 0;
}

void close_this_connect()
{
    LOG_WARNING_TIME("close this connect !");
    g_global_value.in_connecting = 0; 
}

int check_bucket_recovery(int bucket_nr)
{
    return 0;
}

