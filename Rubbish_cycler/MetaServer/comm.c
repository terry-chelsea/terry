/*
 * =====================================================================================
 *
 *       Filename:  comm.c
 *
 *    Description:  communicate with CS...
 *
 *        Version:  1.0
 *        Created:  08/08/13 15:50:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "Debug.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>

typedef struct ThreadInfo Tinfo;
typedef struct config Configuration;
#define LISTENQ   6

int do_check_heartbeat(Configuration *config)
{
}

int do_thread_read(Configuration *config , Tinfo *thread)
{
}

int do_cs_read(Configuration *config , Tinfo *thread)
{
}

void do_exit_rubbish_recycler(Configuration *config , Tinfo *thread)
{
    (void *)thread;
    if(config->comm_fd > 0)
    {
        colse(config->comm_fd);
        config->comm_fd = -1;
    }

    if(config->hash_ids != NULL)
    {
        free(config->hash_ids);
        config->hash_ids = NULL;
    }

    if(config->bitmap != NULL)
    {
        free(config->bitmap);
        config->bitmap = NULL;
    }
}

void start_rubbish_server(Configuration *config , Tinfo *thread)
{
    if(NULL == config || NULL == thread)
    {
        LOG_ERROR("NULL configuration info or thread info , Error ...") ;
        return ;
    }

    struct sockaddr_in addr;
    memset(&addr , 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config->listen_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int fd = socket(AF_INET , SOCK_STREAM , 0);
    if(fd < 0)
    {
        LOG_SYSCALL("Create listen socket failed ...");
        return ;
    }

    if(bind(fd , (struct sockaddr *)&addr , sizeof(addr)) < 0)
    {
        LOG_SYSCALL("Bind address to socket failed ...");
        return ;
    }

    if(listen(fd , LISTENQ) < 0)
    {
        LOG_SYSCALL("Do Listen in socket faile ...");
        return ;
    }
    
    struct pollfd fds[3];
    memset(fds , 0 , sizeof(fds));

    //for listen socket fd...
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    //for pipe read fd to thread...
    fd[1].fd = (thread.main_read)[0];
    fds[1].events = POLLIN;

    //the last one is for accepted connection with CS socket...
    int cur_fds = 2;

    time_t last_check_time = 0;
    while(!(config->exit_flag))
    {
        int readys = poll(fds , cur_fds , check_interval);
        time_t now = time(NULL);
        if(readys < 0)
        {
            LOG_SYSCALL_TIME("Do poll failed ...");
            return ;
        }

        else if(config->check_interval > 0 && 
                now - last_check_time > config->check_interval)
        {
            int ret = do_check_heartbeat(config);

            last_check_time = new;
        }
        
        if(fds[0].revents & POLLIN)
        {
            struct sockaddr_in conn;
            memset(&conn , 0 , sizeof(conn));
            socklen_t connlen = sizeof(conn);
            int connfd = accept(fd , (struct sockaddr *)&conn , &connlen);
            if(connfd < 0)
            {
                LOG_SYSCALL_TIME("Do accept new connection failed ...");
                config->exit_flag = 1;
                close(fd);
                fds[0].fd = 0;
                fds[0].events = 0;
            }
            
            char ip[32] = {0};
            if(inet_ntop(AF_INET , &(conn.sin_addr) , ip , sizeof(ip) - 1) == NULL)
                LOG_SYSCALL_TIME("ntop : Change network address to string failed ...");
            else 
                LOG_INFO_TIME("New connection From %s and port %d ..." , 
                        ip , ntoh(conn.sin_port));

            if(config->comm_fd > 0)
            {
                LOG_WARNING_TIME("New connection when Rubbish recycler is running...");
                //when RC is running , close new connections ...
                close(connfd);
            }
            else 
            {
                LOG_INFO_TIME("Start NEW rubbish recycler socket fd is %d..." , connfd);
                fds[2].fd = connfd;
                fds[2].events = POLLIN;
                ++ cur_fds;
                config->comm_fd = connfd;
            }
        }
        if(fds[1].revents & POLLIN)
        {
            if(do_thread_read(thread) < 0)
            {
                LOG_ERROR_TIME("Error happened when communicating with thread using pipe ...");
                do_exit_rubbish_recycler(config , thread);
                return ;
            }
        }

        if(fds[2].revents & POLLIN)
        {
            if(do_cs_read(config , thread) < 0)
            {
                LOG_ERROR_TIME("Error happened when communicating with CS ...");
                do_exit_rubbish_recycler(config , thread);
                return ;
            }
        }
    }

    LOG_ERROR_TIME("Can not be here ...");
}
