/*
 * =====================================================================================
 *
 *       Filename:  SystemAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 18:06:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "SystemAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "CSConfiguration.h"
#include "Get_rule_task.h"
#include "AgentRegister.h"
#include "Task_manager.h"

SystemAgent::SystemAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(sock , addr , epl) ,
    m_req(NULL)
{

}

SystemAgent::~SystemAgent()
{
    if(m_req != NULL)
    {
        delete m_req;
        m_req = NULL;
    } 
}

void SystemAgent::readBack(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t cmd = header.cmd;

    int ret = -1;
    switch(cmd)
    {
        //增量获取规则信息，如果当前版本较小，回复全部的规则信息 
        case MSG_SYS_RS_UPDATE_MU_HASH :
        {
            ret = get_mu_order_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get order mu rule error !");
            }
            break ;
        }
        case MSG_SYS_RS_UPDATE_SU_HASH :
        {
            ret = get_su_order_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get order su rule error !");
            }
            break ;
        }
        //获取全部的规则信息
        case MSG_SYS_RS_UPDATE_ALL_MU_HASH :
        {
            ret = get_all_mu_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get all mu rule error !");
            }
            break ;
        }
        case MSG_SYS_RS_UPDATE_ALL_SU_HASH :
        {
            ret = get_all_su_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get all su rule error !");
            }
            break ;
        }
        //获取一个桶的分布情况，一般不使用
        case MSG_SYS_RS_GET_MU_BUCKET_ITEM :
        {
            ret = get_mu_bucket_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SytemAgent::get mu bucket rule error !");
            }
            break ;
        }
        case MSG_SYS_RS_GET_SU_BUCKET_ITEM :
        {
            ret = get_su_bucket_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get su bucket rule error !");
            }
            break ;
        }

        default :
            LOG_ERROR("SystemAgent::Undefined cmd type : " + int_to_str(cmd));
            break ;
    }
}

int SystemAgent::generate_all_hash_string(RULE_TYPE type , std::string &out_data)
{
    out_data.clear();
    struct HashRule hash_rule;
    uint32_t current_bucket_num = RulerManager::getInstance()->get_bucket_number(type);

    cstore::pb_MSG_SYS_RS_UPDATE_ALL_MU_HASH_ACK all_mu_hash;
    cstore::pb_MSG_SYS_RS_UPDATE_ALL_SU_HASH_ACK all_su_hash;
    cstore::Hash_Version  *hash_version_ptr;
    cstore::Map_Item      *map_items_ptr;
   
    if(MU_RULER == type)
    {
        hash_version_ptr = all_mu_hash.mutable_hash_version();
    }
    else
    {
        hash_version_ptr = all_su_hash.mutable_hash_version();
    }
    hash_version_ptr->set_hash_version(0);

    for(uint32_t i = 0 ; i < current_bucket_num ; ++ i)
    {
        memset(&hash_rule , 0 , sizeof(hash_rule));
        if(RulerManager::getInstance()->get_hash_rule(i , type , hash_rule) < 0)
        {
            LOG_ERROR("SystemAgent::get rule from ruleManager error !");
            return -1;
        }
    
        //首先获取一个空间
        map_items_ptr = hash_version_ptr->add_map_list();
        for(int j = 0 ; j < MAX_IP_NUM ; ++ j)
        {
            uint32_t ip = hash_rule.ip[j];
            if(INVALID_IP == ip)
                break ;

            map_items_ptr->add_module_ip(ip);
        } 
        map_items_ptr->set_bucket_num(i);
    }
    hash_version_ptr->set_hash_version(RulerManager::getInstance()->get_current_version(type));

    bool ret = false;
    if(MU_RULER == type)
    {
        ret = ((!all_mu_hash.IsInitialized()) || (!all_mu_hash.SerializeToString(&out_data)));
    }
    else
    {
        ret = ((!all_su_hash.IsInitialized()) || (!all_su_hash.SerializeToString(&out_data)));
    }

    if(ret)
    {
        LOG_ERROR("SystemAgent::protobuf initialized error !");
        return -1;
    }
    else
    {
        std::cerr<<"---------------------System info -------------------"<<std::endl;
        std::cerr<<(type == MU_RULER ? all_mu_hash.DebugString() : all_su_hash.DebugString())<<std::endl;
    }

    if(type == MU_RULER)
    {
 //       LOG_EVENT("undate all mu rule info : " + all_mu_hash.DebugString());
    }
    else
    {
//        LOG_EVENT("undate all su rule info : " + all_su_hash.DebugString());
    }

    return 0;
}

int SystemAgent::get_all_mu_rule(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    string input_data(req.ioBuf , header.length);

    std::string out_data;
    int ret = 0 ;
    uint32_t start_version = 0;
    cstore::pb_MSG_SYS_RS_UPDATE_ALL_MU_HASH mu_hash;
    //获取用户当前的版本号，但是不使用，返回全部的规则信息
    if(!mu_hash.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! mu hash request conetnt : " + input_data);
        goto ERR;
    }

    //获取全部的规则的时候，当前的版本参数不使用
    start_version = mu_hash.local_version();
    
    ret = this->generate_all_hash_string(MU_RULER , out_data);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::generate all mu hash rule error !");
        goto ERR ;
    }

    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_ALL_MU_HASH_ACK , RULER_OK , 
            0 , 0 , out_data);

    return 0;

ERR:
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_ALL_MU_HASH_ACK , RS_ERROR , 
            0 , 0 , out_data);

    return -1;
}

int SystemAgent::get_all_su_rule(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    string input_data(req.ioBuf , header.length);

    std::string out_data;
    int ret = 0 ; 
    uint32_t start_version = 0;

    cstore::pb_MSG_SYS_RS_UPDATE_ALL_SU_HASH su_hash;
    if(!su_hash.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! su rule request content : " + input_data);
        goto ERR;
    }

    start_version = su_hash.local_version();

    ret = this->generate_all_hash_string(SU_RULER , out_data);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::generate all Su hash rule error !");
        goto ERR ;
    }

    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_ALL_SU_HASH_ACK , RULER_OK , 
            0 , 0 , out_data);

    return 0;

ERR:
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_ALL_SU_HASH_ACK , RS_ERROR , 
            0 , 0 , out_data);

    return -1;
}

//获取从start开始到当前版本的所有的增量规则的信息
int SystemAgent::generate_plus_order_string(RULE_TYPE type , uint32_t start , std::string &out_data)
{
    out_data.clear();
    uint32_t current_version = RulerManager::getInstance()->get_current_version(type);

    cstore::pb_MSG_SYS_RS_UPDATE_MU_HASH_ACK  mu_order_ack;
    cstore::pb_MSG_SYS_RS_UPDATE_SU_HASH_ACK  su_order_ack;
    cstore::Sys_Order                        *sys_order_ptr = NULL;

    if(MU_RULER == type)
    {
        for(uint32_t i = start ; i <= current_version ; ++ i)
        {
            sys_order_ptr = mu_order_ack.add_sys_order();
            if(generate_one_order_string(type , sys_order_ptr , i) < 0)
            {
                LOG_ERROR("SystemAgent::generate one order string error !");
                return -1;
            }
        }
    }
    else
    {
        for(uint32_t i = start ; i <= current_version ; ++ i)
        {
            sys_order_ptr = su_order_ack.add_sys_order();
            if(generate_one_order_string(type , sys_order_ptr , i) < 0)
            {
                LOG_ERROR("SystemAgent::generate one order string error !");
                return -1;
            }
        }
    }

    bool ret = false ;
    if(MU_RULER == type)
    {
        ret = ((!mu_order_ack.IsInitialized()) || (!mu_order_ack.SerializeToString(&out_data)));
    }
    else
    {
        ret = ((!su_order_ack.IsInitialized()) || (!su_order_ack.SerializeToString(&out_data)));
    }

    if(ret)
    {
        LOG_ERROR("SystemAgent::protobuf initialized error !");
        return -1;
    }

    if(MU_RULER == type)
    {
        LOG_EVENT("update mu order rule info : " + mu_order_ack.DebugString());
    }
    else
    {
        LOG_EVENT("update su order rule info : " + su_order_ack.DebugString());
    }

    return 0;
}

int SystemAgent::generate_one_order_string(RULE_TYPE type , cstore::Sys_Order *sys_order_ptr , int index)
{
    struct OrderRule  order_rule;
    memset(&order_rule , 0 , sizeof(order_rule));
    if(RulerManager::getInstance()->get_order_rule(index , type , order_rule) < 0)
    {
        LOG_ERROR("SystemAgent::get order rule error !");
        return -1;
    }

    std::string order_content;
    if(MIGRATION == order_rule.cmd)
    {
        sys_order_ptr->set_order_header(MU_RULER == type ? MSG_MPC_CS_MU_MIGRATE_BUCKET
                : MSG_MPC_CS_SU_MIGRATE_BUCKET);
        if(generate_migration_string(order_rule.order.migration , order_content) < 0)
        {
            LOG_ERROR("SystemAgent::generate migration string error !");
            return -1;
        }
    }
    else
    {
        sys_order_ptr->set_order_header(MU_RULER == type ? MSG_MPC_CS_MU_EXTENT_BUCKET
                : MSG_MPC_CS_SU_EXTENT_BUCKET);
        if(generate_extent_string(order_rule.order.extent , order_content) < 0)
        {
            LOG_ERROR("SystemAgent::generate extent string error !");
            return -1;
        }
    }
    sys_order_ptr->set_order_content(order_content);
    sys_order_ptr->set_hash_version(order_rule.version);
    
    return 0;
}

int SystemAgent::get_mu_order_rule(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    string input_data(req.ioBuf , header.length);
    std::string out_data;

    int ret = -1;
    uint32_t start_version = 0;
    uint32_t current_version = 0;
    cstore::pb_MSG_SYS_RS_UPDATE_MU_HASH  mu_hash;
    if(!mu_hash.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! mu order request content : " + input_data);
        goto ERR ;
    }

    start_version = mu_hash.local_version();
    current_version = RulerManager::getInstance()->get_current_version(MU_RULER);

    if(start_version >= current_version || start_version < 0)
    {
        LOG_ERROR("SystemAgent::mu current version : " + int_to_str(current_version) 
                + " Request version : " + int_to_str(start_version));
        goto EMPTY_ACK ;
    }

    ret = send_hash_or_order_rule(MU_RULER , start_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::get mu hash rule and order rule error !");
        goto ERR ;
    }
    //返回1 ， 发送全部的规则表
    else if(ret)
    {
        goto SEND_RULE;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_MU_HASH_ACK , RULER_OK , 
           RS_SYS_UPDATE_ORDER , 0 , out_data);

        return 0;
    }

SEND_RULE :
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_MU_HASH_ACK , RULER_OK , 
           RS_SYS_UPDATE_ALL_HASH , 0 , out_data);
    return 0;

EMPTY_ACK :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_MU_HASH_ACK , RS_SYS_INPUT_ERROR , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_MU_HASH_ACK , RS_ERROR , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);

    return -1;
}

int SystemAgent::get_su_order_rule(InReq &req)
{
    int ret = -1;
    MsgHeader header = req.m_msgHeader;
    string input_data(req.ioBuf , header.length);
    std::string out_data;

    uint32_t start_version = 0;
    uint32_t current_version = 0 ;
    cstore::pb_MSG_SYS_RS_UPDATE_SU_HASH  su_hash;
    if(!su_hash.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! su order request content : " + input_data);
        goto ERR ;
    }

    start_version = su_hash.local_version();
    current_version = RulerManager::getInstance()->get_current_version(SU_RULER);
    if(start_version == 0 && current_version == 0)
    {
        CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_SU_HASH_ACK , RULER_OK , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);
        return 0;
    }
    if(start_version >= current_version || start_version < 0)
    {
        LOG_ERROR("SystemAgent::su current version : " + int_to_str(current_version) 
                + " Request version : " + int_to_str(start_version));
        goto EMPTY_ACK ;
    }

    ret = send_hash_or_order_rule(SU_RULER , start_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::get su hash rule and order rule error !");
        goto ERR ;
    }
    //返回1 ， 发送全部的规则表
    else if(ret)
    {
        goto SEND_RULE;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_SU_HASH_ACK , RULER_OK , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);

        return 0;
    }

SEND_RULE :
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_SU_HASH_ACK , RULER_OK , 
            RS_SYS_UPDATE_ALL_HASH , 0 , out_data);

    return 0;
    
EMPTY_ACK :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_SU_HASH_ACK , RS_SYS_INPUT_ERROR , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_UPDATE_SU_HASH_ACK , RS_ERROR , 
            RS_SYS_UPDATE_ORDER , 0 , out_data);

    return -1;
}

//判断发送全部的规则表还是发送增量的规则信息
//返回值为-1表示出错，发送出错信息
//返回值为0说明需要发送增量的规则信息
//返回值为1说明发送全部的规则信息
int SystemAgent::send_hash_or_order_rule(RULE_TYPE type , uint32_t start_version , std::string &out_data)
{
    out_data.clear();

    int ret = -1;
    bool is_get_all_rule = false;
    //根据当前的版本号判断是增量拉去还是全部的规则表的获取
    is_get_all_rule = this->judge_version(start_version , RulerManager::getInstance()->get_current_version(SU_RULER));
    
    //让它执行两次，只要一次执行成功就返回，根据上面的判断进行选择
    for(int i = 0 ; i < 2 ; ++ i)
    {
        //需要获取全部的规则信息
        if(is_get_all_rule)
        {
            ret = generate_all_hash_string(type , out_data);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get all hash error when get order rule !");
                //出错就设置这个标志，然后再次循环，执行增量拉去，再出错，返回错误
                is_get_all_rule = false;
                continue ;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            ret = generate_plus_order_string(type , start_version , out_data);
            if(ret < 0)
            {
                LOG_ERROR("SystemAgent::get order rule error !");
                //下次循环的时候重试拉取全部规则
                is_get_all_rule = true ;
                continue ;
            }
            else
            {
                return 0;
            }
        }
    }

    return -1;
}


bool SystemAgent::judge_version(uint32_t current_version , uint32_t req_version)
{
    return false;
}

int SystemAgent::generate_migration_string(const Migration_Info &info , std::string &migration_string)
{
    cstore::pb_MSG_MPC_CS_SU_MIGRATE_BUCKET  migration_pb;

    migration_pb.set_source_ip(info.src_ip);
    migration_pb.set_destination_ip(info.dest_ip);
    migration_pb.set_bucket_num(info.bucket_nr);

    if((!migration_pb.IsInitialized()) || (!migration_pb.SerializeToString(&migration_string)))
    {
        LOG_ERROR("SystemAgent::protobuf initialized error when migration string !");
        return -1;
    }

    return 0;
}

int SystemAgent::generate_extent_string(const Extent_Info &info , std::string &extent_string)
{
    cstore::pb_MSG_MPC_CS_SU_EXTENT_BUCKET extent_pb;

    extent_pb.set_new_mod(info.new_mod);

    if((!extent_pb.IsInitialized()) || (!extent_pb.SerializeToString(&extent_string)))
    {
        LOG_ERROR("SystemAgent::protobuf initialized error when extent string !");
        return -1;
    }

    return 0;
}

int SystemAgent::get_mu_bucket_rule(InReq &req)
{
    MsgHeader header = req.m_msgHeader ;
    std::string input_data(req.ioBuf , header.length);
    std::string out_data;
    struct HashRule hash_rule;

    int ret = -1 ; 
    uint32_t bucket_nr = 0;
    uint32_t bucket_number = 0;
    cstore::pb_MSG_SYS_RS_GET_MU_BUCKET_ITEM  pb_req;
    if(!pb_req.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! mu bucket request content : " + input_data);
        goto ERR ;
    }

    bucket_nr = pb_req.bucket_num();
    bucket_number = RulerManager::getInstance()->get_bucket_number(MU_RULER);
    if(!(bucket_nr >= 0 && bucket_nr <= bucket_number))
    {
        LOG_ERROR("SystemAgent::Invalid Input : bucket number : " 
                + int_to_str(bucket_number) + " MU Request bucket : " 
                + int_to_str(bucket_nr));
        goto EMPTY_ACK ;
    }
    
    memset(&hash_rule , 0 , sizeof(hash_rule));
    ret = RulerManager::getInstance()->get_hash_rule(bucket_nr , MU_RULER , hash_rule);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::get mu rule of bucket " + int_to_str(bucket_nr) + " error !");
        goto ERR ;
    }

    if(generate_single_bucket_hash_string(SU_RULER , hash_rule , out_data) < 0)
    {
        LOG_ERROR("SystemAgent::generate su hash rule to protobuf string error !");
        goto ERR ;
    }

    CSTCPAgent::reply_request(MSG_SYS_RS_GET_MU_BUCKET_ITEM_ACK , RULER_OK , 
            0 , 0 , out_data);
    return 0;

EMPTY_ACK :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_GET_MU_BUCKET_ITEM_ACK , RS_SYS_INPUT_ERROR , 
            0 , 0 , out_data);
    
    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_GET_MU_BUCKET_ITEM_ACK , RS_ERROR , 
            0 , 0 , out_data);

    return -1;
}

int SystemAgent::get_su_bucket_rule(InReq &req)
{
    MsgHeader header = req.m_msgHeader ;
    std::string input_data(req.ioBuf , header.length);
    std::string out_data;
    struct HashRule hash_rule;
    memset(&hash_rule , 0 , sizeof(hash_rule));
    int ret = -1 ; 

    uint32_t bucket_nr = 0 ;
    uint32_t bucket_number = 0 ;
    cstore::pb_MSG_SYS_RS_GET_SU_BUCKET_ITEM  pb_req;
    if(!pb_req.ParseFromString(input_data))
    {
        LOG_ERROR("SystemAgent::parse protobuf error ! su bucket request content : " + input_data);
        goto ERR ;
    }

    bucket_nr = pb_req.bucket_num();
    bucket_number = RulerManager::getInstance()->get_bucket_number(SU_RULER);
    if(!(bucket_nr >= 0 && bucket_nr <= bucket_number))
    {
        LOG_ERROR("SystemAgent::Invalid Input : bucket number : " 
                + int_to_str(bucket_number) + " SU Request bucket : " 
                + int_to_str(bucket_nr));
        goto EMPTY_ACK ;
    }
    
    ret = RulerManager::getInstance()->get_hash_rule(bucket_nr , SU_RULER , hash_rule);
    if(ret < 0)
    {
        LOG_ERROR("SystemAgent::get su rule of bucket " + int_to_str(bucket_nr) + " error !");
        goto ERR ;
    }
    
    if(generate_single_bucket_hash_string(SU_RULER , hash_rule , out_data) < 0)
    {
        LOG_ERROR("SystemAgent::generate su hash rule to protobuf string error !");
        goto ERR ;
    }

    CSTCPAgent::reply_request(MSG_SYS_RS_GET_SU_BUCKET_ITEM_ACK , RULER_OK ,
            0 , 0 , out_data);

    return 0;

EMPTY_ACK :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_GET_SU_BUCKET_ITEM_ACK , RS_SYS_INPUT_ERROR ,
            0 , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(MSG_SYS_RS_GET_SU_BUCKET_ITEM , RS_ERROR ,
            0 , 0 , out_data);

    return -1;
}

int SystemAgent::generate_single_bucket_hash_string(RULE_TYPE type , const struct HashRule &rule , std::string &out_data)
{
    uint32_t ip = INVALID_IP;
    bool ret = false;
    cstore::pb_MSG_SYS_RS_GET_MU_BUCKET_ITEM_ACK mu_bucket_ack;
    cstore::pb_MSG_SYS_RS_GET_SU_BUCKET_ITEM_ACK su_bucket_ack;
    cstore::Map_Item                            *map_item_ptr = NULL;
    if(MU_RULER == type)
    {
        map_item_ptr = mu_bucket_ack.mutable_ip_list();
    }
    else
    {
        map_item_ptr = su_bucket_ack.mutable_ip_list();
    }
    
    for(int i = 0 ; i < MAX_IP_NUM ; ++ i)
    {
        ip = rule.ip[i];
        if(INVALID_IP == ip)
            break ;
        
        map_item_ptr->add_module_ip(ip);
    }
    map_item_ptr->set_bucket_num(rule.bucket_nr);
    if(MU_RULER == type)
    {
        ret = ((!mu_bucket_ack.IsInitialized()) || (!mu_bucket_ack.SerializeToString(&out_data)));
    }
    else
    {
        ret = ((!su_bucket_ack.IsInitialized()) || (!su_bucket_ack.SerializeToString(&out_data)));
    }
    if(ret)
    {
        LOG_ERROR("SystemAgent::protobuf Initialized error !");
        return -1;
    }
    else
    {
        std::cerr<<"---------------system info-----------------"<<std::endl;
        std::cerr<<(MU_RULER == type ? mu_bucket_ack.DebugString() : su_bucket_ack.DebugString())<<std::endl;
    }
    if(MU_RULER == type)
        LOG_EVENT("mu bucket info : " + mu_bucket_ack.DebugString());
    else
        LOG_EVENT("su bucket info : " + su_bucket_ack.DebugString());

    return 0;
}

int SystemAgent::after_get_new_rule()
{
    int ret = -1;
    if(GET_MU_ORDER == m_wait_type)
    {
        ret = get_mu_order_rule(*m_req);
        if(ret < 0)
        {
            LOG_ERROR("SystemAgent::get order mu rule error !");
            return -1;
        }
    }
    else if(GET_SU_ORDER == m_wait_type)
    {
        ret = get_su_order_rule(*m_req);
        if(ret < 0)
        {
            LOG_ERROR("SystemAgent::get order mu rule error !");
            return -1;
        }
    }
    else 
    {
        LOG_ERROR("SystemAgent::get undefined wait type !");
        return -1;
    }

    return 0;
}
