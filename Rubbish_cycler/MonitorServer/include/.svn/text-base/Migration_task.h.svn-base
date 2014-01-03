/*
 * =====================================================================================
 *
 *       Filename:  Migration_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 17:43:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_MIGRATION_TASK_H_
#define _H_MIGRATION_TASK_H_

#include <iostream>
#include <cstring>
#include <tr1/functional>

#include "Task.h"
#include "SCAgent.h"
#include "RulerManager.h"
#include "WorkItem.h"

//对于MU的迁移，只需要发送一个报文，然后更新规则并且通知RS拉取更新的规则
//对于SU的迁移，需要两个报文，第一个报文回复之后就更新规则，然后通知RS拉取
//更新的规则，最后再发送另外一个报文完成。
//操作流程如下（以SU为例）
//1、MPC发送SU桶迁移报文，并指定桶号和源IP和目的IP
//2、CS收到这个报文之后保存信息并创建MigrationTask,设定回调函数A，执行init 
//并且指定回调函数B
//3、在init中会向目的IP连接并且发送桶迁移报文，SU完成迁移工作
//4、SU回复完成报文，CS收到之后会回调函数B，回到MigrationTask中
//5、回调函数B创建WorkItem修改数据库，并设置回调函数C。
//6、线程执行完成执行回调函数C，更新内存中的规则表，然后通知RS更新规则
//7、RS收到报文之后会连接CS拉取最新的规则。
//8、创建Agent通知源IP删除迁移的桶，设置回调函数D
//9、收到源SU的回复之后调用回调函数D，这时候迁移工作完成，调用回调函数A
//10、在回调函数A中一般会向MPC回复SU桶迁移的完成情况。
//
//以上步骤基本上是顺序的，只是第6步和第8步可以不顺序执行，但是第6步的实现
//是个问题，本想使用心跳回复通知RS更新规则，但是这样很难做到及时的通知
//所以还是使用RS一个单独的监听套接口监听CS更新规则的请求。
//6和8步是完全异步的，即使其中任意一个失败将不会对另外的操作有任何影响

//由于MU的桶迁移全部的任务交给MU完成，所以只需要发送一个报文，完成之后立即执行更新数据库
//操作，成功更新之后再通知RS更新规则
//但是对于SU，需要两阶段完成，并且第二节点和通知RS的操作并行执行，因此需要两个标志
//标志着这两个操作是否已经完成，如果都完成了就执行上层传递的回调函数了。
//并且需要保存它们的错误，如果任何一个出现错误，都会向上层回复失败~
//但是失败的原子性却不能提供
//如果MU或者SU桶迁移成功，但是CS的更新规则失败，那么就会产生不一致的情况，这时候应该
//宕机处理
//如果MU或者SU执行失败，数据库也不会被更新，这时候是一致的
//如果MU或者SU操作成功，但是通知RS更新规则失败，这时候RS的规则将会和CS不一致，应该让失败的
//RS宕机处理
//
//这里的确可以设置重试次数，但是这里没有尝试
//
//可以同时执行多个桶迁移工作。

class MigrationTask : public Task
{
    public :
        MigrationTask(uint32_t agent_index , RULE_TYPE type , struct Migration_Info info);
        ~MigrationTask();

        //初始化工作，连接目的节点的IP
        int init(std::tr1::function<void (bool , RULE_TYPE , struct Migration_Info)> ret_callback);

    private :

        //对于MU和SU，在收到目的节点操作完成之后执行的回调函数
        void done_dest_migration(void *req , RET_TYPE type , void *);

        //对于SU而言，在收到源节点上的桶之后执行的回调函数
        void done_del_bucket(void *req , RET_TYPE type , void *);

        int migration_exec_back(bool ret);

        int update_rule_and_db();

        void after_update_db(bool ret);

        void do_callback(bool ret);

    private :
        RULE_TYPE  m_type;              //桶迁移的类型
        uint32_t m_agent_index;         //向MU或者SU发送请求的agent句柄

        struct Migration_Info m_mig_info;      //本次迁移操作的信息

        std::tr1::function<void (bool , RULE_TYPE , Migration_Info)>  m_callback_func;

        WorkItem *m_item;
};

#endif
