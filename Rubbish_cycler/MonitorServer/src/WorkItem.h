/*
 * =====================================================================================
 *
 *       Filename:  WorkItem.h
 *
 *    Description:  下发到线程池的任务基类 
 *
 *        Version:  1.0
 *        Created:  11/06/12 11:01:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_WORK_ITEM_H_
#define _H_WORK_ITEM_H_

#include <iostream>
#include <cstring>
#include <string>
#include <tr1/functional>

#include "RulerManager.h"

class WorkItem
{
    public : 
        WorkItem() :
            m_exec_result(false) 
        {

        }
        virtual ~WorkItem()
        {

        }

        virtual void init(std::tr1::function<void (bool)> cb)
        {
            m_cb = cb;
        }

        void set_callback(std::tr1::function<void (bool)> cb)
        {
            m_cb = cb;
        }

        bool get_result()
        {
            return m_exec_result;
        }

        virtual void set_operation_result(bool ret , const std::string &res)
        {}

        virtual int pre_process()
        {
            return 0;
        }

        virtual int process() = 0;

        virtual int process_after();

        void do_callback()
        {
            m_cb(m_exec_result);
        }

    protected :
        bool m_exec_result;
        
        std::tr1::function<void (bool)> m_cb;
};

#endif
