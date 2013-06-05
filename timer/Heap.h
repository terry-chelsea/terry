/*
 * =====================================================================================
 *
 *       Filename:  Heap.h
 *
 *    Description:  a heap for timer insert / delete and select the smallest item
 *
 *        Version:  1.0
 *        Created:  04/19/13 16:13:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_HEAP_INSIDE_H_
#define _H_HEAP_INSIDE_H_

#include <vector>
#include <list>
#include "Timer.h"

typedef unsigned long long int UINT64;

class Timer;
class TimerHeap
{
    public :
        TimerHeap();

        ~TimerHeap();

        bool add_timer_to_heap(Timer *);          //向heap中添加一个Timer...

        UINT64 get_root_time();                  //获取最小的超时时间，用于设置timerfd

        void get_and_remove_expire_timers(UINT64 , std::list<Timer *> &);                    //根据当前的时候确定超时的定时器

        bool delete_timer_from_heap(Timer *);   //删除一个指定的Timer

        void display_all_timrs();
    private :
        void adjust_heap();
        void adjust_heap_2(int);
        int find_elememt_in_vector(Timer *tmr);
        void swap_element(Timer *& , Timer *&);

    private : 
        std::vector<Timer *>        m_root;            //heap的根节点
        int                    m_heap_size;
        struct timeval         m_last_expire;     //上一次超时的时间
};

#endif
