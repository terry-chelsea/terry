
#include <iostream>
#include <vector>
#include "Heap.h"
#include "Timer.h"
#include <cstdlib>
#include <tr1/functional>
#include <sys/time.h>

using namespace std;

#define COUNTERS 20

class TEST
{
    public : 
        void create_timers(int num)
        {
            for(int i = 0 ; i < num ; ++ i)
            {
                int sec = 0;
                int usec = rand() % COUNTERS;
                struct timeval tm = {sec , usec};
                struct timeval inv = { 0 , 0};

                Timer *tmr = new Timer(tm , inv , tr1::bind(&TEST::cb_func , this , 
                            tr1::placeholders::_1) , NULL , 1);

                m_array.push_back(tmr);
            }
        }

        ~TEST()
        {
            int sz = m_array.size();
            for(int i = 0 ; i < sz ; ++ i)
            {
                delete m_array[i];
            }
        }

        int cb_func(void *arg)
        {
            return 0;
        }

        vector<Timer *> m_array;
};

int main()
{
    srand(time(NULL));
    TEST *tst = new TEST();

    tst->create_timers(COUNTERS);
    TimerHeap *heap = new TimerHeap();

    vector<Timer *>::iterator it = tst->m_array.begin();
    while(it != tst->m_array.end())
    {
        if(heap->add_timer_to_heap(*it))
            cerr<<"Need change timeout !"<<endl;
        ++ it;

        if((it - tst->m_array.begin()) % 5 == 0)
        {
            heap->display_all_timrs();
        }

    }

    getchar();

    for(int i = 0 ; i < 5 ; ++ i)
    {
        /*  
        heap->delete_timer_from_heap(tst->m_array[i]);
        */

        list<Timer *> temp_vec;
        heap->get_and_remove_expire_timers((UINT64)(10 + i) , temp_vec);
        heap->display_all_timrs();

        for(list<Timer *>::iterator it = temp_vec.begin() ; it != temp_vec.end() ; ++it)
            cerr<<"get expire timer : "<<(*it)->get_expire_time()<<endl;
        
        getchar();
    }

    delete tst;

    return 0;
}
