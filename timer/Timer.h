/*
 * =====================================================================================
 *
 *       Filename:  Timer.h
 *
 *    Description:  basic timer object in manager heap
 *
 *        Version:  1.0
 *        Created:  04/19/13 15:38:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TIMER_H_INSIDE_
#define _H_TIMER_H_INSIDE_

#include <tr1/functional>
#include <sys/time.h>

#define MAX_TIMES 0x7FFFFFFF

typedef enum
{
    ONE_SHOT ,        //只超时一次
    UNLIMIT ,         //循环的超时
    LIMITED ,         //定时有限次
    MAX_TYPE
}_TYPE;

typedef unsigned long long int UINT64;
typedef long long int TIMER_INDEX;

class Timer
{
    public : 
        Timer(struct timeval , struct timeval , std::tr1::function<int (void *)> , void * , int);
        ~Timer();

        void action();                       //超时时候执行
    
        UINT64 get_expire_time();    //获取超时时间

        void set_timeout(struct timeval tv);

        bool continue_timer();           //在每次定时器超时并执行完回调函数之后，该函数
                                        //被调用以查看是否需要重新将该定时器插入到列表中
		void set_interval_time(struct timeval );
		
        void set_last_times(int cnt);

        int  get_last_times();

        int  get_last_error();

		void set_index(TIMER_INDEX);
		TIMER_INDEX get_index();

    private :
        UINT64    m_expire_time;             //超时值精确到微妙,这里保存的应该是相对值!
		UINT64    m_interval_time;
		std::tr1::function<int (void *)> m_cb;  //回调函数，用户保证回调不会出错
        void  *m_para;
        _TYPE m_timer_type;                 //定时器的类型
        int   m_last_times;                 //定时器超时的次数
        int   m_last_error;                 //上一次执行的错误情况
		TIMER_INDEX m_index;


    public : 
        static UINT64 change_timeval(struct timeval tm)
        {
            return ((tm.tv_sec * 1000000) + tm.tv_usec);
        }
};

#endif
