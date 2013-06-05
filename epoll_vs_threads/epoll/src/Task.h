#ifndef _H_BASE_TASK_H_
#define _H_BASE_TASK_H_

#include "BaseReq.h"

class Task;
typedef void (*Callback) (Task *);

class Task
{
    public :
        Task();
        ~Task();

        virtual int init(struct TPara para , Callback cb) = 0;

        void init_load_buf(uint32_t);
        
        int get_task_result();

        struct TPara &get_result_para();

        void do_callback();
    protected :
        struct TPara m_para;      //execute parameter for each task ...
        
        int m_ret;                  //return result to upper...
        Callback m_cb;
};

#endif
