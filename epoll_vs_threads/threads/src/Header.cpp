/*
 * =====================================================================================
 *
 *       Filename:  Header.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/25/13 19:29:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "Header.h"
#include "log.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define LISTENQ 50000
struct info
{
    client_work func;
    int fd;
};

void *start_client(void *args)
{
    struct info *info = (struct info *)args;
    pthread_detach(pthread_self());
    
    if(info->func(info->fd) < 0)
    {
        LOG_ERROR("client function for fd %d works error" , info->fd);
        return NULL;
    }

    static int counter = 0;

    if(info != NULL)
    {
        free(info);
        info = NULL;
    }

    LOG_INFO("Destory of thread counter %d" , ++ counter);

    return NULL;
}

int start_mutil_thread_server(char * ip , unsigned short port , client_work func)
{
    int fd = socket(AF_INET , SOCK_STREAM , 0);
    if(fd < 0)
        LOG_SYSCALL_EXIT("create socket error");

    struct sockaddr_in serv;
    memset(&serv , 0 , sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if(NULL == ip)
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
    else
    {
        if(inet_pton(AF_INET , ip , &(serv.sin_addr)) < 0)
            LOG_SYSCALL_EXIT("change ip %s to net type error" , ip);
    }

    if(bind(fd , (struct sockaddr *)&serv , sizeof(serv)) != 0)
        LOG_SYSCALL_EXIT("bind socket to address error");

    if(listen(fd , LISTENQ) < 0)
        LOG_SYSCALL_EXIT("listen to this address error");

    int connfd = -1;
    struct sockaddr_in client;
    socklen_t len;
    pthread_t tid = 0;
    while(1)
    {
        memset(&client , 0 , sizeof(client));
        connfd = accept(fd , (struct sockaddr*)&client , &len);
        if(connfd < 0)
        {
            LOG_SYSCALL("accept new connection error");
            break;
        }

        static int counter = 0;
        printf("connection number : %d\n" , ++ counter);
        //can not use LOG_INFO here !
        
        //HERE malloc will be free in start_client...
        struct info *info = (struct info*)malloc(sizeof(*info));
        memset(info , 0 , sizeof(*info));
        info->func = func;
        info->fd = connfd;
        if(pthread_create(&tid , NULL , start_client , (void *)info) < 0)
        {
            LOG_SYSCALL("create new thread for new connection error");
            break;
        }
    }

    LOG_EXIT("SYSTEM ERROR WHILE PROCESS EXECUTING!!!");
    return 0;
}

int readn_blocking(int fd , char *buf , size_t len)
{
    int rlen = -1;
    int read_in = 0;
    int last_len = len;
    while(last_len > 0)
    {
        rlen = read(fd , buf + read_in , last_len);
        if(rlen < 0)
        {
            if(EINTR == errno)
                continue ;
            else 
            {
                LOG_SYSCALL("read from fd %d and has read %d need read %d" , 
                        fd , read_in , last_len);
                return -1;
            }
        }
        else if(0 == rlen)
            return 0;
        else 
        {
            read_in += rlen;
            last_len -= rlen;
        }
    }

    return read_in;
}

int writen_blocking(int fd , char *buf , size_t len)
{
    int wlen = -1;
    int write_out = 0;
    int last_out = len;
    while(last_out > 0)
    {
        wlen = write(fd , buf , last_out);
        if(wlen <= 0)
        {
            if(EINTR == errno)
                continue ;
            else if(EPIPE == errno)
                return 0;
            else 
            {
                LOG_SYSCALL("write to fd %d and has write %d need write %d" , 
                        fd , write_out , last_out);
                return -1;
            }
        }
        else 
        {
            last_out -= wlen;
            write_out += wlen;
        }
    }

    return write_out;
}

#define IP_LENGTH 32
char *get_peer_info(int fd , unsigned short *port)
{
    struct sockaddr_in addr;
    memset(&addr , 0 , sizeof(addr));
    socklen_t len = 0;
    if(getpeername(fd , (struct sockaddr *)&addr , &len) < 0)
    {
        LOG_SYSCALL("Get peer address error of fd %d" , fd);
        return NULL;
    }

    *port = ntohs(addr.sin_port);
    char *ip = (char *)malloc(IP_LENGTH);
    memset(ip , 0 , IP_LENGTH);
    if(inet_ntop(AF_INET , &(addr.sin_addr) , ip , IP_LENGTH) < 0)
    {
        LOG_SYSCALL("Change net address to string error");
        return NULL;
    }

    return ip;
}
