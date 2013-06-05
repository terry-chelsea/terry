/*
 * =====================================================================================
 *
 *       Filename:  Timer_manager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 21:23:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TIMER_MANAGER_H_
#define _H_TIMER_MANAGER_H_

#include <tr1/functional>
#include <sys/time.h>
#include <map>
#include "Heap.h"

//Timer是对外不可见的，所有的接口通过timer的一个id实现。
//因此对任意一个Timer的操作都需要通过TimerManager，后者在map上查找
//然后找到对应的Timer执行相应的操作
class Timer;
class TimerHeap;
class TimerManager
{
    public :
        TimerManager();
        ~TimerManager();

        int initialize();

        //创建一个定时器，返回值是代表定时器的一个整数，-1表示错误
        //添加一个绝对时间的定时器，超时时间为第一个参数，精确到微妙
        TIMER_INDEX add_timer(struct timeval , std::tr1::function<int (void *)> , void *);

        //添加一个绝对时间的定时器，超时时间由第一个参数决定，精确到秒
        TIMER_INDEX add_timer(time_t , std::tr1::function<int (void *)> , void *);

        //添加一个绝对时间的定时器，超时时间由第一个参数决定，精确到秒
        TIMER_INDEX add_timer(struct tm , std::tr1::function<int (void *)> , void *);

        //添加一个相对当前时间的定时器，第一个参数是秒数，第二个参数是微妙数
        //第五个参数是超时的次数，如果是超时无限次，设置为-1
        TIMER_INDEX add_timer(int , int , std::tr1::function<int (void *)> , void * , int = 1);

        //删除一个指定的定时器，参数为id
        //从heap中删除该定时器，不能在超时执行期间执行该函数
        int delete_timer(TIMER_INDEX);

        //提前调用一个指定定时器的回调函数，调用之后定时器不会被删除
        //这个函数也不能在超时执行期间执行.
        int expire_now(TIMER_INDEX);

        //提前调用一个指定定时器的回调函数，调用之后定时器会被删除
        //算作一次超时，超时次数减去1
        int expire_now_and_delete(TIMER_INDEX);

        int get_timer_fd()
        {
            return m_fd;
        }

        //在timerfd可读之后执行该函数，执行所有超时的timer的回调函数。
        int expire_all();

    //对于定时器不能执行修改操作，如果出现修改的需求，可以先进行delete然后在add
        int change_expire_time(TIMER_INDEX , bool , struct timeval , int = 1);

        //根据index得到对应的Timer对象，出错返回NULL...
        Timer *get_timer_from_index(TIMER_INDEX);
        
    private :
        TIMER_INDEX get_next_index()
        {
            return ++m_next_index;
        }

		TIMER_INDEX add_and_get_index(Timer *tmr , bool );

        int set_new_timeout();

    private :
        TimerHeap                      *m_heap;           //保存所有定时器的堆
        std::map<TIMER_INDEX , Timer *> m_active_timer;   //所有活跃的定时器
        int                             m_fd;             //timerfd函数操作的描述符
        TIMER_INDEX                     m_next_index;     //用于生成一个定时器的下一个索引号
};

#endif
