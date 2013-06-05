/*
 * =====================================================================================
 *
 *       Filename:  Thread.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/29/12 10:44:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "Thread.h"

Thread::Thread(string name)
    :s_name(name)
{
    n_tid = -1;
    f_thread = NULL;
}

Thread::~Thread()
{
    if(n_tid != -1)
        pthread_join(n_tid , NULL);
}

void Thread::run(THREAD_FUNC *threadStart , void *arg)
{
    if(pthread_create(&n_tid , NULL , threadStart , arg) < 0)
    {
        perror("pthread create error : ");
        n_tid = -1;
        return ;
    }
}

