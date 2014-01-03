/*
 * =====================================================================================
 *
 *       Filename:  Get_rule_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 14:51:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_GET_RULE_TASK_H_
#define _H_GET_RULE_TASK_H_

#include "Task.h"
#include "SystemAgent.h"
#include <iostream>
#include <list>
#include "RulerManager.h"
#include "SCAgent.h"

#include "../common/protocol/protocol.h"

//增量拉取规则Task,提供给RS使用，它会在接收到CS拉取报文的通知之后被CSNoticeAgent创建
//它完成向CS增量拉取规则，并且完成之后唤醒所有可能等待的客户端请求的Agent
//对于拉取规则可能出现这样的情况：
//假设拉取规则的报文成为报文A，CS通知更新规则的报文成为报文B
//CS首先通知RS更新规则，向RS发送一个B报文B1，然后RS创建出这种Task，它会创建Agent
//向CS发送报文A拉取规则，如果在报文A发送出去之后CS再次需要更新规则，再次向RS发送报文B2
//无论报文B2什么时候到，报文A的回复报文能够包含最新的规则
//但是考虑这样的情况，假设报文A已经发出去了，并且得到了回复，但是这时候CS再次向RS发送
//更新规则的报文B3，并且B3比报文A的回复报文先到，如果不再向CS拉取规则将会产生不一致
//
//因此，为了保证一致，即使某次更新规则拉取不到最新的规则（因为上一次已经拉取了）
//还是需要对CS的报文B进行计数，并且保证每次只有一个拉取报文的操作
//于是在收到报文B的时候首先判断是否已经有拉取报文的操作（全局的Task标志），
//如果没有就创建这个全局的Task，如果存在，那么只增加拉取规则通知的计数，上层在完成一次
//更新之后需要查看是否已经完成了全部的更新请求。

//之所以使用等待队列的方式是考虑用户一般拉取规则是在定位出错的时候，这很可能是因为CS规则
//的更新引起的，这时候用户向RS拉取规则，但是RS此时并没有更新到最新的规则，那么RS回复给用户
//的依旧是旧的版本，所以可能再次定位出错，因此等待到RS的更新完成对用户的定位有利
//这种策略可以通过配置完成，甚至可以通过用户的更新报文设置

class GetRuleTask : public Task
{
    public :
        //拉取规则的类型
        GetRuleTask(uint32_t agent_index , RULE_TYPE type);
        ~GetRuleTask();

        int init(std::tr1::function<void (bool)> ret_callback);

        //添加一个Agent到等待队列上，完成拉取之后唤醒所有的等待Agent
        void wait_agent(uint32_t agent_index)
        {
            m_wait_client_agent.push_back(agent_index);
        }
    
    private :
        //拉取到新规则之后的回到，由SCAgent进行
        void done_get_new_rule(void *req , RET_TYPE type , void *);

        //成功获取到更新的目录
        int get_new_rule_ack(InReq *req);

        //唤醒所有等待的Agent，这里需要dynamic_cast转换，统一的接口
        void wakeup_all_wait_request();

        //将所有得到的规则更新到RS的规则表中
        void apply_all_new_rule(std::list<cstore::Sys_Order>& order_vec);
        
        //处理桶迁移或者桶扩展操作到RS的规则表上
        void deal_with_new_order_rule(uint32_t cmd , std::string content 
                , uint32_t version);

    private :
        //客户端是否需要等待规则更新的返回，这是一个全局的，也可以通过配置
        bool m_is_wait_client;             //是否需要等待请求的回复
        std::list<uint32_t> m_wait_client_agent;  //所以等待的agent
        uint32_t m_rule_agent_index;       //获取规则的Agent，由Task创建
        RULE_TYPE  m_type;                 //获取规则的类型
};

#endif
