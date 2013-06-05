/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.h
 *
 *    Description:  实现线程池的功能，自带两根管道，用于于主线程通信
 *
 *        Version:  1.0
 *        Created:  11/06/12 10:35:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_THREAD_POOL_H_
#define _H_THREAD_POOL_H_

#include "Thread.h"
#include "../common/Singleton.h"
#include <list>
#include <iostream>
#include <cstring>
#include <string>
#include <tr1/functional>

#define DEFAULT_THREAD_NUM   4
#define MIN_THREAD_NUM       2

class WorkItem
{
    public : 
        WorkItem();
        ~WorkItem();

        void init(std::tr1::function<void (int)> cb , 
                std::tr1::function<int (void *)> proc , void *para)
        {
            m_cb = cb;
            m_process = proc;
            m_para = para;
        }

        void do_callback()
        {
            m_cb(m_exec_result);
        }

        bool get_result()
        {
            return m_exec_result;
        }

        int pre_process();

        void process();

        int process_after();

        void *get_exec_result();
        void set_exec_result(void *ret);

    protected :
        int m_exec_result;
        
        std::tr1::function<void (int)> m_cb;
        std::tr1::function<int (void *)> m_process;
        void * m_para;
        bool   exec_after;
};

class ThreadPool : public Singleton<ThreadPool> 
{
    public : 
        ThreadPool();
        ~ThreadPool();

        //主线程读管道的套接字，一般为非阻塞
        int get_main_write_handle()
        {
            return m_main_write_fd;
        }
        //主线程写管道的套接字，一般为非阻塞
        int get_main_read_handle()
        {
            return m_main_read_fd;
        }
        int start(int num = DEFAULT_THREAD_NUM);
        
        int post_request(WorkItem *item);

        //暂停所有线程，使用信号量实现
        int stop();

        int restart();

        //向线程池中添加一些线程
        int add_thread(int num = 1);

        //暂停部分线程
        int stop_thread(int num = 1);

        int get_thread_num()
        {
            return m_thread_num;
        }

    protected :
        class ThreadHandler : public Thread
        {
            public : 
                ThreadHandler(ThreadPool *pool);

            protected :
                void run();

            private :
                ThreadPool *m_thread_pool;
        };

    private :
        int m_thread_num;
        int m_main_write_fd;
        int m_main_read_fd;
        int m_thread_write_fd;
        int m_thread_read_fd;

        std::list<Thread *> m_thread_list;
        Mutex m_mutex;

        //workitem allocater
};


#endif
