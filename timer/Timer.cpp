
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "Timer.h"

Timer::Timer(struct timeval tm , struct timeval inv , std::tr1::function<int (void *)> cb , void *para , int ct)
    :m_cb(cb) , 
    m_para(para) ,
    m_last_times(ct) 
{
    this->m_expire_time = change_timeval(tm);
	this->m_interval_time = change_timeval(inv);

    if(0 == ct)
        m_timer_type = UNLIMIT;
    else if(ct > 1)
        m_timer_type = LIMITED ;
    else 
    {
        m_last_times = 1;
        m_timer_type = ONE_SHOT;
    }
    
    m_last_error = 0;
}

Timer::~Timer()
{

}

void Timer::action()
{
    m_last_error = this->m_cb(m_para);
}

UINT64 Timer::get_expire_time()
{
    return m_expire_time;
}

void Timer::set_timeout( struct timeval tm)
{
    m_expire_time = change_timeval(tm);
}

void Timer::set_interval_time(struct timeval tm)
{
	m_interval_time = change_timeval(tm);
}

bool Timer::continue_timer()
{
	bool ret = false;
    if(m_timer_type == UNLIMIT)
        ret = true ;
    else 
    {
        m_last_times --;
        ret = (m_last_times > 0);
    }

	if(ret)
	{
		struct timeval now;
		gettimeofday(&now , NULL);
		m_expire_time = change_timeval(now) + m_interval_time;
	}

	return ret;
}

int Timer::get_last_times()
{
    return this->m_last_times;
}

void Timer::set_last_times(int cnt)
{
    if(0 == cnt)
        m_timer_type = UNLIMIT;
    else if(cnt > 1)
	{
		m_last_times = cnt;
        m_timer_type = LIMITED ;
	}
    else 
    {
        m_last_times = 1;
        m_timer_type = ONE_SHOT;
    }
}

int Timer::get_last_error()
{
    return m_last_error;
}

void Timer::set_index(TIMER_INDEX index)
{
	m_index = index;
}

TIMER_INDEX Timer::get_index()
{
	return m_index;
}
