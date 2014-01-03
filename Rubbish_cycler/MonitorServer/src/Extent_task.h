/*
 * =====================================================================================
 *
 *       Filename:  Extent_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 22:09:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_EXTENT_TASK_H
#define _H_EXTENT_TASK_H

#include <iostream>
#include <cstring>
#include <list>
#include <map>
#include <tr1/functional>

#include "Task.h"
#include "SCAgent.h"
#include "RulerManager.h"
#include "WorkItem.h"
//#include "Notice_RS_task.h"

//对于桶扩展操作，需要设置一个全局的标志，在桶迁移的过程中任何的桶迁移和桶扩展操作
//都会被拒绝
//另外对于桶扩展操作，MU和SU执行的流程是相同的
//首先更新数据库和内存的规则表，之后再通知RS拉取更新之后的规则信息
//如果失败就可以直接回复MPC桶扩展失败
//否则再通知每一个有心跳连接的节点进行桶扩展操作
//等待每一个对端回复的报文，只要收到所有的回复之后就向MPC回复桶扩展成功
//如果有回复桶迁移操作失败的节点，重复向该节点发送命令，直到成功为止！
//
//对于每一个桶迁移的命令，得到回复之后会携带一个index表示哪一个回复了
//对于执行成功的节点，将其从所有的节点agent链表中移除
//如果没有执行成功，将它移动到另外一个链表上，从源链表上移除
//这样持续重试第二个链表上的节点Agent，当第一个链表为空的时候就回复MPC桶迁移成功！
//重新设置全局的标志状态
//
//全局的标志通过Task_manager中的指针标志，对于ExtentTask可以有三种状态（SU与MU独立）
//1、空闲状态：表示当前没有桶扩展操作，可以执行任何操作
//2、扩展状态：表示当前正在执行桶迁移，并且没有回复MPC，不能执行桶扩展和桶迁移操作
//3、重试状态：表示当前已经回复了MPC，但是有未完成的节点，正在重试，此时可移植性桶迁移
//但是桶迁移的源和目的桶不能涉及正在重试的节点
//
//所有有冲突的操作，都以MSG_CS_EXTENT错误码进行回复

//但是这样可能存在一个问题，如果有需要重试的节点A，那么在Task接收到所有的回复之后
//会回调设定的回调函数，回复MPC ， 之后MPC会关闭这个连接，对于的Agent将会被析构
//但是这个Task还是依然存在，执行者重试操作，此时它不在于Agent有任何关系，
//况且那个Agent已经不存在了

typedef enum
{
    FREE = 0 ,    //这个状态可以通过判断Task_manager的指针，因此不不使用
    EXTENTING = 1 , //全局桶扩展状态
    RETRY = 2       //有一些桶正在重试，但是已经执行了回调
}EXTENT_STATE;

class ExtentTask : public Task
{
    public :
        ExtentTask(uint32_t agent_index , RULE_TYPE type , struct Extent_Info info);
        ~ExtentTask();

        int init(std::tr1::function<void (bool , RULE_TYPE , struct Extent_Info)> ret_callback);
        
        bool can_exec_migration(uint32_t src , uint32_t dest);

    private :
        void done_modify_rule(bool ret);

        int create_agent_and_init(uint32_t node_ip , uint32_t &index_ret);

        void done_extent_ack(void *req , RET_TYPE type , void *data);

        void give_up_this_node(uint32_t agent_index);

        void retry_do_extent(uint32_t agent_index);

        void agent_exec_extent_back(uint32_t agent_index , bool);

        bool remove_agent_from_retry_list(uint32_t agent_index);

        bool remove_agent_from_extent_list(uint32_t agent_index);

        void do_callback(bool ret);

        void do_last_work(bool , const std::string &);

        void do_last_back(bool);

    public : 
        //判断是否回收Task，如果没有Agent存在说明可以回收
        bool can_recycler_task()
        {
            return (m_extent_agent_list.empty() && m_retry_agent_list.empty());
        }

    private :
        RULE_TYPE m_type ;
        Extent_Info m_ext_info;

        std::list<uint32_t>  m_extent_agent_list;
        std::list<uint32_t>  m_retry_agent_list;

        //防止Agent析构了，那么重试的时候就找不到对应的IP了
        //它不会被析构，只是在向每个节点发送命令的时候添加
        std::map<uint32_t , uint32_t>  m_index_ip_map;
//        NoticeRSTask  *m_notice_task;
        EXTENT_STATE m_state;
    
        bool m_extent_ret;
        std::tr1::function<void (bool , RULE_TYPE , struct Extent_Info)> m_callback_func;

        WorkItem *m_item;
};

#endif
