/*
 * =====================================================================================
 *
 *       Filename:  Thread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/08/13 12:20:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include "Debug.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct ThreadInfo
{
    pthread_t thread_id;
    int  main_write[2];
    int  main_read[2];
}Tinfo;

typedef struct req
{
    unsigned int cmd;
    void *para;
}REQ;

void *start_thread(void *args)
{
    Tinfo *thread = (Tinfo *)args;
    
    int readfd = thread.main_write[0];
    while(1)
    {
        REQ req;
        int len = read(readfd , &req , sizeof(req));
        if(len != sizeof(req))
        {
            LOG_SYSCALL_TIME("Do read start request failed ...");
            break;
        }

        if(req.cmd = )
    }
    
    return NULL;
}

Tinfo *create_work_thread()
{
     Tinfo *info = (Tinfo *)malloc(sizeof(*info));
     if(NULL == info)
     {
         LOG_ERROR("Allocate memory size %d failed ..." , sizeof(*info));
         return NULL;
     }
     memset(info , 0 , sizeof(*info));

     if(pipe(info->main_read) < 0)
     {
         LOG_SYSCALL("Create main read pipe for thread failed ...");
         goto PIPE1;
     }
     if(pipe(info->main_write) < 0)
     {
         LOG_SYSCALL("Create main write pipe for thread failed ...");
         goto PIPE2;
     }

     pthread_t tid;
     if(pthread_create(&tid , NULL , start_thread , (void *)info) < 0)
     {
         LOG_SYSCALL("Create work thread failed ...");
         goto THREAD ;
     }
     info->thread_id = tid;
     LOG_INFO_TIME("Create work thread , thread id : %llu ..." , (unsigned long long)tid);

     return info;

THREAD :
     close(info->main_write[0]);
     close(info->main_write[1]);

PIPE2 :
     close(info->main_read[0]);
     close(info->main_read[1]);

PIPE1 :
    free(info);
    info = NULL;
}

void  stop_work_thread(Tinfo *thread);
{
    //in default , work thread is cancelable...
    pthread_cancel(thread->thread_id);
    close(info->main_write[0]);
    close(info->main_write[1]);
    close(info->main_read[0]);
    close(info->main_read[1]);
}

