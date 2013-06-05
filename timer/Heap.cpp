
#include "Heap.h"
#include <iostream>

TimerHeap::TimerHeap() :
    m_root()  , 
    m_heap_size(0)
{
    gettimeofday(&m_last_expire , NULL);
}

TimerHeap::~TimerHeap()
{
}

bool TimerHeap::add_timer_to_heap(Timer *tmr)
{
    //是否需要改变定时器的超时时间
    UINT64 expire = 0;
    if(!m_root.empty())
        expire = m_root[0]->get_expire_time();

    m_root.push_back(tmr);
    m_heap_size ++;

    adjust_heap();
    if(0 == expire)
        return true ;

    if(expire > m_root[0]->get_expire_time())
        return true;
    else 
        return false;
}

UINT64 TimerHeap::get_root_time()
{
    if(m_root.empty())
        return -1;

    else 
        return m_root[0]->get_expire_time();
}

void TimerHeap::get_and_remove_expire_timers(UINT64 tm , std::list<Timer *>& tm_list)
{
    int backup_size = m_heap_size;
    while(m_heap_size > 0)
    {
        if(m_root[0]->get_expire_time() < tm)
        {
            UINT64 expi = m_root[0]->get_expire_time();
            std::cerr<<"Root expire time : "<<expi / 1000000<<"."<<expi%1000000<<std::endl;
            swap_element(m_root[0] , m_root[m_heap_size - 1]);
            m_heap_size --;

            adjust_heap_2(0);
        }
        else 
            break;
    }

    std::vector<Timer *>::iterator it = m_root.begin();
    tm_list.assign(it + m_heap_size , it + backup_size);
    m_root.erase(it + m_heap_size , it + backup_size);
}

bool TimerHeap::delete_timer_from_heap(Timer *tmr)
{
    UINT64 expire = 0;
    if(m_root.empty())
        return false;
    else 
        expire = m_root[0]->get_expire_time();

    int ret = find_elememt_in_vector(tmr);
    if(ret != -1)
    {
        swap_element(m_root[ret] , m_root[m_heap_size - 1]);
        
        m_heap_size --;
        adjust_heap_2(ret);

        m_root.erase(m_root.begin() + m_heap_size);
        if(m_root[0]->get_expire_time() != expire)
            return true;
    }

    return false;
}

void TimerHeap::adjust_heap_2(int start)
{
    int max_index = m_heap_size - 1;
    int k = start;
    int i = 0;
    while((k << 1) + 1 < max_index)
    {
        if((k << 1) + 2 >= max_index)
            i = ((k << 1) + 1);
        else
        {
            i = (k << 1) + 2;
            if(m_root[i - 1]->get_expire_time() < m_root[i]->get_expire_time())
                i -= 1;
        }

        if(m_root[i]->get_expire_time() < m_root[k]->get_expire_time())
        {
            swap_element(m_root[i] , m_root[k]);
            k = i;
        }
        else 
            break;
    }
}

void TimerHeap::adjust_heap()
{
    if(1 == m_heap_size)
        return ;

    int i = m_heap_size - 1;

    int par = (i - 1) >> 1;
    while(1)
    {
        if(i == 0)
            break;

        if(m_root[par]->get_expire_time() > m_root[i]->get_expire_time())
        {
            swap_element(m_root[i] , m_root[par]);

            i = par;
            par = (i - 1) >> 1;
        }
        else
            break;
    }
}

int TimerHeap::find_elememt_in_vector(Timer *tmr)
{
    std::vector<Timer *>::iterator it = m_root.begin();
    std::vector<Timer *>::iterator it_end = m_root.end();

    while(it != it_end)
    {
        if(*it == tmr)
            break;

        ++ it;
    }
    if(it != it_end)
        return it - m_root.begin();
    else 
        return -1;
}

/*  
int TimerHeap::find_elememt_in_vector(Timer *tmr)
{
    UINT64 expire = tmr->get_expire_time();
    int high = m_heap_size - 1;
    int low = 0;
    int middle = 0;
    UINT64 comp = 0;

    while(low < high)
    {
        middle = (low + high) / 2;
        comp = m_root[middle]->get_expire_time();

        if(expire < comp)
        {
            high = middle - 1;
        }
        else if(expire > comp)
        {
            low = middle + 1;
        }
        else 
            break;
    }

    //说明没有找到对应的timer
    if(low >= high)
        return -1;

    //再找到之后还需要比较timer指针，因为之上比较的只是超时时间
    //这是因为可能存在多个相等的超时时间的定时器
    for(low = middle ; m_root[low]->get_expire_time() != expire ; -- low)
    {
        if(m_root[low] == tmr)
            break;
    }
    if(m_root[low] == tmr)
        return low;

    for(high = middle ; m_root[high]->get_expire_time() != expire ; ++ high)
    {
        if(m_root[high] == tmr)
            break;
    }
    if(m_root[high] == tmr)
        return high;

    return -1;
}
*/

void TimerHeap::display_all_timrs()
{
    int i = 0;
    for(i = 0 ; i < m_heap_size ; ++ i)
    {
        std::cerr<<"Timer "<<i<<" expire time : "<<m_root[i]->get_expire_time()<<std::endl;
    }
}

void TimerHeap::swap_element(Timer *&one , Timer *&ano)
{
    Timer *tmr = one;
    one = ano;
    ano = tmr;
}
