#include "Timer_manager.h"
#include "Timer.h"
#include "Heap.h"
#include <iostream>
#include <sys/timerfd.h>
#include <cstring>
#include <errno.h>
#include <time.h>
#include <list>
#include <assert.h>

#define SECOND_PER  1000000

#define LOG_ERR  (std::cerr)
#define LOG_INFO (std::cout)
#define LOG_END  (std::endl)

/* Timer Manager implement */
TimerManager::TimerManager()
    :m_heap(NULL) , 
    m_active_timer() , 
    m_fd(-1) , 
    m_next_index(0)
{

}

//析构所有的Timer，执行所有的回调函数
TimerManager::~TimerManager()
{
    if(m_heap != NULL)
    {
        delete m_heap;
        m_heap = NULL ;
    }

    Timer *tmr = NULL;
    std::map<TIMER_INDEX , Timer *>::iterator it;
    for(it = m_active_timer.begin() ; it != m_active_timer.end() ; ++ it)
    {
        tmr = it->second;
        tmr->action();
        delete tmr;
    }

    if(m_fd >= 0)
        close(m_fd);
}

int TimerManager::initialize()
{
    int fd = timerfd_create(CLOCK_REALTIME , TFD_CLOEXEC);
    if(fd < 0)
    {
        LOG_ERR<<"TimerManager::System call timerfd_create error : "<<strerror(errno)<<LOG_END;
        return -1;
    }

    m_heap = new TimerHeap();
    if(NULL == m_heap)
    {
        LOG_ERR<<"TimerManager::Create heap for timer error !"<<LOG_END;
		close(fd);
        return -1;
    }

    m_fd = fd;
    return 0;
}

TIMER_INDEX TimerManager::add_timer(struct timeval tm , std::tr1::function<int (void *)> cb , void *para)
{
	struct timeval now , gap;
	gettimeofday(&now , NULL);

	if(tm.tv_usec < now.tv_usec)
	{
		gap.tv_usec = tm.tv_usec - now.tv_usec + SECOND_PER;
	    gap.tv_sec = tm.tv_sec - 1 - now.tv_sec;
	}
    else 
    {
        gap.tv_sec = tm.tv_usec - now.tv_usec;
        gap.tv_sec = tm.tv_sec - now.tv_sec;
    }
    if(gap.tv_sec < 0)
    {
        LOG_ERR<<"TimerManager::Must set timer value postive !"<<LOG_END;
        return -1;
    }

    Timer *tmr = new Timer(tm , gap , cb , para , 1);
    if(NULL == tmr)
    {
        LOG_ERR<<"TimerManager::Create new timer object error !"<<LOG_END;
        return -1;
    }

	return add_and_get_index(tmr , true);
}

TIMER_INDEX TimerManager::add_and_get_index(Timer *tmr , bool if_index)
{
    //add timer to heap and adjust the heap
    int ret = m_heap->add_timer_to_heap(tmr);
    if(ret < 0)
    {
        LOG_ERR<<"TimerManager::add timer to heap error !"<<LOG_END;
		delete tmr;
        return -1;
    }

    //return 1 means need reset the next timerfd expire time..
    if(1 == ret)
    {
        if(set_new_timeout() < 0)
        {
            LOG_ERR<<"set New timeout error !"<<LOG_END;
            return -1;
        }
    }

    TIMER_INDEX index = 0;
    if(if_index)
    {
        index = get_next_index();
	    while(1)
	    {
		    std::pair<std::map<TIMER_INDEX , Timer *>::iterator , bool> ret = 
			    m_active_timer.insert(std::make_pair(index , tmr));
		    if(ret.second)
			    break;
		    index = get_next_index();
	    }
        tmr->set_index(index);
    }
    
    return index;
}

int TimerManager::set_new_timeout()
{
    UINT64 expire = m_heap->get_root_time();
    struct itimerspec spec , old;
    spec.it_value.tv_sec = expire / SECOND_PER;
    spec.it_value.tv_nsec = (expire % SECOND_PER) * 1000;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;
    if(timerfd_settime(m_fd , TFD_TIMER_ABSTIME , &spec , &old) < 0)
    {
        LOG_ERR<<"Set new timer expire absolute value error !"<<LOG_END;
        return -1;
    }
    else 
        LOG_INFO<<"Old expire time seconds : "<<old.it_value.tv_sec<<
            " and nseconds : "<<old.it_value.tv_nsec<<LOG_END;

    return 0;
}

TIMER_INDEX TimerManager::add_timer(time_t tmt , std::tr1::function<int (void *)> cb , void *para)
{
    struct timeval tv;
	tv.tv_sec = tmt;
	tv.tv_usec = 0;

    return add_timer(tv , cb , para);
}

TIMER_INDEX TimerManager::add_timer(struct tm tm , std::tr1::function<int (void *)> cb , void *para)
{
    time_t tmt = mktime(&tm);
    if(tmt < 0)
    {
        LOG_ERR<<"TimerManager::modify tm struct to time_t error !"<<LOG_END;
        return -1;
    }

    return add_timer(tmt , cb , para);
}

TIMER_INDEX TimerManager::add_timer(int sec , int usec , std::tr1::function<int (void *)> cb , void *para , int cnt)
{
	if(sec < 0 || usec < 0)
	{
		LOG_ERR<<"Relative timer value MUST be postive !"<<LOG_END;
		return -1;
	}
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;

	struct timeval now;
	gettimeofday(&now , NULL);
	int add_sec = 0;
	now.tv_usec += usec;
	if(now.tv_usec >= SECOND_PER)
	{
		add_sec += 1;
		now.tv_usec -= SECOND_PER;
	}
	now.tv_sec += sec + add_sec;

    Timer *tmr = new Timer(now , tv , cb , para , cnt);
    if(NULL == tmr)
    {
        LOG_ERR<<"TimerManager::Create new timer object error !"<<LOG_END;
        return -1;
    }

	return add_and_get_index(tmr , true);
}

Timer *TimerManager::get_timer_from_index(TIMER_INDEX index)
{
    std::map<TIMER_INDEX , Timer *>::iterator it;
    it = m_active_timer.find(index);
    if(m_active_timer.end() == it)
		return NULL;

	return it->second;
}

int TimerManager::delete_timer(TIMER_INDEX timer_index)
{
	Timer *tmr = get_timer_from_index(timer_index);
    if(NULL == tmr)
    {
        LOG_INFO<<"TimerManager::Timer of index "<<timer_index<<" not existing !"<<LOG_END;
        return -1;
    }
    else 
        m_active_timer.erase(timer_index);

    bool ret = m_heap->delete_timer_from_heap(tmr);
    if(ret)
    {
        if(set_new_timeout() < 0)
        {
            LOG_ERR<<"Set new timeout error when delete timer !"<<LOG_END;
        }
    }

    delete tmr;
	tmr = NULL;
    return 0;
}

int TimerManager::expire_now(TIMER_INDEX  timer_index)
{
	Timer *tmr = get_timer_from_index(timer_index);
    if(NULL == tmr)
    {
        LOG_INFO<<"TimerManager::Timer of index "<<timer_index<<" not existing !"<<LOG_END;
        return -1;
    }
    
    tmr->action();

	return 0;
}

int TimerManager::expire_now_and_delete(TIMER_INDEX timer_index)
{
	Timer *tmr = get_timer_from_index(timer_index);
    if(NULL == tmr)
    {
        LOG_INFO<<"TimerManager::Timer of index "<<timer_index<<" not existing !"<<LOG_END;
        return -1;
    }

    tmr->action();

    bool ret = m_heap->delete_timer_from_heap(tmr);
    if(ret)
    {
        if(set_new_timeout() < 0)
        {
            LOG_ERR<<"Set new timeout error when delete timer !"<<LOG_END;
        }
    }
	m_active_timer.erase(timer_index);

    delete tmr;
	tmr = NULL;

    return 0;
}

int TimerManager::change_expire_time(TIMER_INDEX index , bool type, struct timeval tv , int cnt)
{
    if(tv.tv_sec < 0 || tv.tv_usec < 0)
    {
        LOG_ERR<<"TimerManager::Must set timer value postive !"<<LOG_END;
        return -1;
    }

	Timer *tmr = get_timer_from_index(index);
    if(NULL == tmr)
    {
        LOG_INFO<<"TimerManager::Timer of index "<<index<<" not existing !"<<LOG_END;
        return -1;
    }

    m_heap->delete_timer_from_heap(tmr);
    
	if(type)
	{
		tmr->set_timeout(tv);
	}
	else
	{
		struct timeval now;
		gettimeofday(&now , NULL);
		int add_sec = 0;
		now.tv_usec += tv.tv_usec;
		if(now.tv_usec >= SECOND_PER)
		{
			add_sec += 1;
			now.tv_usec -= SECOND_PER;
		}
		now.tv_sec += tv.tv_sec + add_sec;

    	tmr->set_timeout(now);
		tmr->set_interval_time(tv);
		tmr->set_last_times(cnt);
	}

	if(add_and_get_index(tmr , false) < 0)
	{
		LOG_ERR<<"Add and change timer info error !"<<LOG_END;
		return -1;
	}

    return 0;
}

int TimerManager::expire_all()
{
	std::list<Timer *> expire_list;

    struct timeval now;
    gettimeofday(&now , NULL);
    LOG_INFO<<"In Timer manager NOW is "<<now.tv_sec<<"."<<now.tv_usec<<LOG_END;
	m_heap->get_and_remove_expire_timers(Timer::change_timeval(now) , expire_list);
	
	assert(!expire_list.empty());
	std::list<Timer *>::iterator it = expire_list.begin();
	std::list<Timer *>::iterator it_end = expire_list.end();
	Timer *tmr = NULL;
	while(it != it_end)
	{
		tmr = *it;
		tmr->action();
		if(tmr->continue_timer())
			m_heap->add_timer_to_heap(tmr);
		else 
		{
			m_active_timer.erase(tmr->get_index());
			delete tmr;
			tmr = NULL;
		}

        ++ it;
	}

	UINT64 expire_tm = m_heap->get_root_time();
	struct itimerspec spec;
    if(UINT64(-1) == expire_tm)
    {
        LOG_INFO<<"All Timer erased , Waiting for new Timer !"<<LOG_END;
        return 0;
    }

	spec.it_value.tv_sec = expire_tm / SECOND_PER;
	spec.it_value.tv_nsec = (expire_tm % SECOND_PER) * 1000;
	spec.it_interval.tv_sec = 0;
	spec.it_interval.tv_nsec = 0;

    if(timerfd_settime(m_fd , TFD_TIMER_ABSTIME , &spec , NULL) < 0)
	{
		LOG_ERR<<"Set new timer expire after expire error !"<<LOG_END;
		return -1;
	}

	return 0;
}
