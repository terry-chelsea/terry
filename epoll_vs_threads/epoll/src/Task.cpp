
#include <cstring>
#include "Task.h"

Task::Task() : 
    m_ret(0) ,
    m_cb(NULL) 
{
    memset(&m_para , 0 , sizeof(m_para));
}

Task::~Task()
{
}

int Task::init(struct TPara para , Callback cb)
{
    m_ret = 0;
    m_para = para;
    m_cb = cb;

    return 0;
}

int Task::get_task_result()
{
    return m_ret;
}

struct TPara &Task::get_result_para()
{
    return m_para;
}

void Task::do_callback()
{
    m_cb(this);
}

void Task::init_load_buf(uint32_t cmd)
{
    m_para.header.cmd = cmd;
    m_para.header.length = 0;
    m_para.load = NULL;
}
