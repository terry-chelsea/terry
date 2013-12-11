/*
 * =====================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:06:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Timer.h"
#include "Epoll.h"

#define STR_ERROR strerror(errno)

int create_timer(int interval_value , int epoll_fd)
{
    int timer_fd = timerfd_create(CLOCK_REALTIME , 0);
    if(timer_fd < 0) 
    {
        LOG_ERROR("In create_timer : timerfd_create error : %s" , STR_ERROR);
        return -1;
    }
    struct timespec interval;
    interval.tv_sec = interval_value;
    interval.tv_nsec = 0;

    struct itimerspec spec;
    spec.it_value = interval;
    spec.it_interval = interval;

    if(timerfd_settime(timer_fd , TFD_TIMER_ABSTIME , &spec , NULL) < 0)
    {
        LOG_ERROR("In create_timer : timerfd_settime error : %s" , STR_ERROR);
        close(timer_fd);
        return -1;
    }

    if(add_to_epoll(epoll_fd , timer_fd , EPOLLIN , NULL) < 0)
    {
        LOG_ERROR("In create_timer : all timer %d to epoll error !" , timer_fd);
        close(timer_fd);
        return -1;
    }

    return timer_fd;
}

int read_timer(int fd , int *times)
{
    uint64_t expired_times = 0;
    if(read(fd , &expired_times , sizeof(expired_times)) != sizeof(expired_times))
    {
        LOG_ERROR("In deal_with_send_heartbeat : read timer fd error : %s" , STR_ERROR);
        return -1;
    }         

    *times = expired_times;

    return 0;
}
