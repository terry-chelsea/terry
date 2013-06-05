
#include "Timer_manager.h"
#include <iostream>
#include <sys/epoll.h>
#include <cstdlib>
#include <sys/time.h>
#include <tr1/functional>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

using namespace std;

static TimerManager *tmanager = NULL;

#define TIMES 2
#define PER_TIME 1
#define ALL_TIMERS (TIMES * PER_TIME)

static int counters = 0;

class TEST
{
    public : 
        TEST()
        {
            struct timeval tm ;
            tm.tv_sec = rand() % 10;
            tm.tv_usec = rand() % 10000;

            int times = rand() % 6 + 1;
            m_index = tmanager->add_timer(tm.tv_sec , tm.tv_usec , tr1::bind(&TEST::cb_func , this , 
                        tr1::placeholders::_1) , this , times);
            if(m_index < 0)
            {
                cerr<<"WA hoo , create timer failed !"<<endl;
            }
            else 
            {
                struct timeval now;
                gettimeofday(&now , NULL);
                m_start = Timer::change_timeval(now);
                cerr<<"A alerm id "<<m_index<<" will expired "<<times<<" times and per "<<tm.tv_sec<<"."<<tm.tv_usec<<" and start "<<now.tv_sec<<"."<<now.tv_usec<<endl;
            }
            m_expire = Timer::change_timeval(tm);
        }

        ~TEST()
        {
        }

        int cb_func(void *arg)
        {
            static int counter = 1;
            UINT64 expire = m_start + m_expire;
            struct timeval now;
            gettimeofday(&now , NULL);

            cerr<<"Alarm id "<<m_index<<" ideal expire timeout : "<<expire / 1000000<<"."
                <<expire % 1000000<<" -- and NOW : "<<now.tv_sec<<"."<<now.tv_usec<<" Counter "<<counter++<<endl;
            m_start = Timer::change_timeval(now);

            return 0;
        }

        TIMER_INDEX  m_index;
        UINT64 m_expire;
        UINT64 m_start;
};

int main()
{
    srand(time(NULL));
    tmanager = new TimerManager();
    if(tmanager->initialize() < 0)
    {
        cerr<<"Timer manager initialize error !"<<endl;
        return -1;
    }

    /*  
    int epoll_fd = epoll_create(10000);
    if(epoll_fd < 0)
    {
        cerr<<"create epoll error !"<<endl;
        return -1;
    }
    */

    /*  
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = tmanager;

   if(epoll_ctl(epoll_fd , EPOLL_CTL_ADD , tmanager->get_timer_fd() , &ev) < 0)
   {
       cerr<<"add to epoll error !"<<endl;
       return -1;
   } 
   */

    int fd = tmanager->get_timer_fd();

//    vector<TEST *>  all_test;
    for(int i = 0 ; i < PER_TIME ; ++ i)
    {
        TEST *tst = new TEST();
        (void)tst;
//        all_test.push_back(tst);

        ++ counters;
    }

    while(1)
    {
        int64_t ret;
        int rr = read(fd , &ret , sizeof(ret));
        if(rr < 0)
        {
            cerr<<"read error : "<<strerror(errno)<<endl;
            return -1;
        }

        tmanager->expire_all();

        if(counters < ALL_TIMERS)
        {
            for(int i = 0 ; i < PER_TIME ; ++ i)
            {
                TEST *tst = new TEST();
                (void)tst;
//                all_test.push_back(tst);

                ++ counters;
            }
        }
    }
}
