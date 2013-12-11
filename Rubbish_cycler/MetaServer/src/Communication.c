/*
 * =====================================================================================
 *
 *       Filename:  Communication.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:51:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Communication.h"
#include "Epoll.h"

int create_pipes(int epoll_fd)
{
    int pipe_fds[2];
    if(pipe(pipe_fds) < 0)
    {
        LOG_ERROR("create pipe 1 error : %s" , strerror(errno));
        return -1;
    }
    (g_global_value.thread_to_main)[0] = pipe_fds[0];
    (g_global_value.thread_to_main)[1] = pipe_fds[1];

    /*  
    if(setNonblock(pipe_fds[0]) < 0)
    {
        LOG_ERROR("set fd %d to nonblock error!" , pipe_fds[0]);
        return -1;
    } 
    */

    if(add_to_epoll(epoll_fd , pipe_fds[0] , EPOLLIN , NULL) < 0)
    {
        LOG_ERROR("add thread to main read fd %d to epoll error!" , pipe_fds[0]);
        return -1;
    }

    if(pipe(pipe_fds) < 0)
    {
        LOG_ERROR("create pipe 2 error : %s" , strerror(errno));
        close_pipe(g_global_value.thread_to_main);
        return -1;
    }
    (g_global_value.main_to_thread)[0] = pipe_fds[0];
    (g_global_value.main_to_thread)[1] = pipe_fds[1];

    if(pipe(pipe_fds) < 0)
    {
        LOG_ERROR("create pipe 3 error : %s" , strerror(errno));
        close_pipe(g_global_value.main_to_thread);
        close_pipe(g_global_value.thread_to_main);
        return -1;
    }
    (g_global_value.wakeup_and_block)[0] = pipe_fds[0];
    (g_global_value.wakeup_and_block)[1] = pipe_fds[1];

    return 0;
}

void close_pipe(int *pipe_fds)
{
    close(pipe_fds[0]);
    close(pipe_fds[1]);
}


//需要通知子线程阻塞
int notice_thread_blocking()
{
    int notice_word = STOP_CHAR;

    if(write_to_thread_blocking(g_global_value.main_to_thread[1] , &notice_word , sizeof(notice_word)) < 0)   //向这个管道写入阻塞字符         
    {
        LOG_ERROR("In notice_thread_blocking : write block word %d to thread error !" , notice_word);
        return -1;
    }

    return 0;
}

int notice_thread_working()
{
    char notice_word = CONT_CHAR;

    if(write_to_thread_blocking(g_global_value.wakeup_and_block[1] , &notice_word , sizeof(notice_word)) < 0)
    {
        LOG_ERROR("In notice_thread_working : write continue word %c to thread error !" , notice_word);
        return -1;
    }

    return 0;
}

int notice_thread_new_bucket(int bucket_nr)
{
    if(write_to_thread_blocking(g_global_value.main_to_thread[1] , &bucket_nr , sizeof(bucket_nr)) < 0)
    {
        LOG_ERROR("In notice_thread_new_bucket : write bucket No.%d to thread error !" , bucket_nr);
        return -1;
    }

    return 0;
}

int notice_thread_start_recycle()
{
    int notice_word = START_CHAR;
    if(write_to_thread_blocking(g_global_value.main_to_thread[1] , &notice_word , sizeof(notice_word)) < 0)
    {
        LOG_ERROR("In notice_thread_start_recycle : write thread start recycle %d error !" , notice_word);
        return -1;
    }

    return 0;
}

//子线程完成了一个桶，需要将这个桶号发送给主线程
int deal_with_thread_notice(int fd)
{
    int bucket_nr;
    int read_ret = read_from_thread_blocking(fd , &bucket_nr);
    if(read_ret < 0)
    {
        LOG_ERROR("In deal_with_thread_notice : read data from pipe error !");
        return -1;
    }
    else if(1 == read_ret)
    {
        LOG_WARNING_TIME("In deal_with_thread_notice : thread close pipe !!!");
        return 1;
    }

    if(FINISH_ALL == bucket_nr)
    {
        g_global_value.I_finish = 1;
        return 0;
    }

    if(list_append_value(g_global_value.finish_bucket_list , bucket_nr) < 0)
    {
        LOG_ERROR("In deal_with_thread_notice : add new bucket No.%d to finish error !" , bucket_nr);
        return -1;
    }

    return 0;
}

//查看是否子线程需要暂停，返回0说明不需要暂停，否则继续
//返回值有三个：
//返回-1说明需要子线程阻塞！
//返回0说明主线程不需要子线程阻塞
//返回1说明主线程关闭了管道
int thread_check_waiting(int *read_value)
{
    int read_fd = g_global_value.main_to_thread[0];
    
    int need_wait = 0;
    int read_ret = -1;
RE_READ : 
    read_ret = read(read_fd , &need_wait , sizeof(need_wait));
    if(read_ret < 0)
    {
        if(EINTR == errno)
            goto RE_READ;
        else if(EAGAIN == errno)                 //非阻塞读取，返回EAGAIN说明不需要暂停
        {
            *read_value = -1;
            return 0;
        }
        else
        {
            LOG_ERROR("In check_waiting : nonblocking read pipe1 error : %s" , strerror(errno));
            return -1;
        }
    }
    else if(0 == read_ret)
    {
        LOG_WARNING_TIME("In check_waiting : Never Happen ! main close pipe1!!!");
        return  1;
    }
    else if(STOP_CHAR == need_wait)                  //需要暂停
    {
        return -1;
    }
    else 
    {
        *read_value = need_wait;
        return 0;
    }

    return 0;
}

int thread_wait_main_wakeup()
{
    char restart_char = 0;
    int  read_fd = g_global_value.wakeup_and_block[0];

    int read_ret = -1;
RE_RRAD:
    read_ret = read(read_fd , &restart_char , sizeof(restart_char));
    if(read_ret < 0)
    {
        if(EINTR == errno)
            goto RE_RRAD;
        else
        {
            LOG_ERROR("In thread_wait_main_wakeup : read pipe3 fd %d error : %s" , read_fd , strerror(errno));
            return -1;
        }
    }
    else if(0 == read_ret)
    {
        LOG_ERROR_TIME("In thread_wait_main_wakeup : Never Happen ! main close pipe3!!!");
        return 1;
    }
    else
    {
        if(CONT_CHAR == restart_char)
            return 0;
        else
        {
            LOG_INFO("Read from pipe3 , read : %c" , restart_char);
            goto RE_RRAD;
        }
    }
    return 0;
}

