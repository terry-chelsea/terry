/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.cpp
 *
 *    Description:  线程池，只提供读写的管道
 *
 *        Version:  1.0
 *        Created:  11/06/12 10:49:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "ThreadPool.h"
#include "util.h"
#include "../common/debug.h"

ThreadPool::ThreadPool() :
    m_thread_num(0) ,
    m_main_write_fd(-1) , 
    m_main_read_fd(-1) ,
    m_thread_write_fd(-1) , 
    m_thread_read_fd(-1)
{
    
}

ThreadPool::~ThreadPool()
{
}

ThreadPool::ThreadHandler::ThreadHandler(ThreadPool *pool)
    :m_thread_pool(pool)
{

}

void ThreadPool::ThreadHandler::run()
{
    WorkItem *p_work_item = NULL;
    int read_fd = m_thread_pool->m_thread_read_fd;
    static unsigned int request_counter = 0;

    //套接字无效的时候，直接返回
    if(read_fd < 0)
        return ;

    //循环的读管道
    while(1)
    {
        int ret = util::io::readn(read_fd , static_cast<void *>(&p_work_item) 
                 , sizeof(void *));
        if(ret == -1)
        {
            LOG_ERROR("ThreadPool::ThreadHandler::read pipe error !");
            break ;
        }
        else if(ret != sizeof(void *))
        {
            LOG_ERROR("ThreadPool::ThreadHandler::main thread close this pipe !");
            break ;
        }

        LOG_INFO("read request counter : %d" , ++ request_counter);

        p_work_item->pre_process();
        p_work_item->process();
        p_work_item->process_after();
    }
}

int ThreadPool::start(int num)
{
    //子线程的写管道，主线程读
    int thread_fd[2];
    //主线程的写管道，线程池中线程读
    int main_fd[2];

    int effect_thread_num = 0;
    ThreadHandler *thread_handler = NULL;
    int ret = -1;
    ret = pipe(thread_fd);
    if(ret < 0)
    {
        LOG_SYSCALL("ThreadPool::Create pipe for thread error !");
        return -1;
    }

    ret = pipe(main_fd);
    if(ret < 0)
    {
        LOG_SYSCALL("ThreadPool::Create pipe for main error !");
        goto CLOSE_THREAD_FD;
    }

    //设置每个描述符的作用
    m_main_read_fd = thread_fd[0];
    m_main_write_fd = main_fd[1];
    m_thread_read_fd = main_fd[0];
    m_thread_write_fd = thread_fd[1];

    if(num < MIN_THREAD_NUM)
        num = MIN_THREAD_NUM;

    for(int i = 0 ; i < num ; ++ i)
    {
        thread_handler = new ThreadHandler(this);
        ret = thread_handler->start();
        if(ret < 0)
        {
            LOG_ERROR("ThreadPool::Start new thread error !");
        }
        else
        {
            m_thread_list.push_back(thread_handler);
            ++ effect_thread_num;
        }
    }

    //没有启动任意一个线程，返回错误
    if(0 == effect_thread_num)
    {
        LOG_ERROR("ThreadPool::Can not start any thread !");
        goto CLOSE_MAIN_FD;
    }
    else
    {
        m_thread_num = effect_thread_num;
    }

    return 0;

CLOSE_MAIN_FD :
    close(main_fd[0]);
    close(main_fd[1]);

CLOSE_THREAD_FD :
    close(thread_fd[0]);
    close(thread_fd[1]);

    return -1;
}

int ThreadPool::post_request(WorkItem *item)
{
    int ret = util::io::writen(m_thread_write_fd , &item , sizeof(void *));
    if(ret < 0)
    {
        LOG_ERROR("ThreadPool::write request back error !");
        return -1;
    }
    else
    {
//        std::cerr<<"write a workitem success from threadpool !"<<std::endl;
    }

    return 0;
}

int ThreadPool::stop()
{
    return 0;
}

int ThreadPool::restart()
{
    return 0;
}

int ThreadPool::stop_thread(int num)
{
    return 0;
}

int ThreadPool::add_thread(int num)
{
    return 0;
}
