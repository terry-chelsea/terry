/*
 * =====================================================================================
 *
 *       Filename:  RSClientAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 21:28:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Log_warp.h"
#include "Lib_tool.h"
#include "RSClientAgent.h"
#include "../common/protocol/protocol.h"

RSClientAgent::RSClientAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(sock , addr , epl)
{

}

RSClientAgent::~RSClientAgent()
{

}

void RSClientAgent::readBack(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t cmd = header.cmd;

    int ret = -1;
    switch(cmd)
    {
        case MSG_RS_CS_UPDATE_ALL_MU_HASH :
        {
            ret = get_all_mu_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("RSClientAgent::get all mu rule error !");
            }
            break ;
        }
        case MSG_RS_CS_UPDATE_MU_HASH :
        {
            ret = get_mu_order_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("RSClientAgent::get all su rule error !");
            }
            break ;
        }
        case MSG_RS_CS_UPDATE_ALL_SU_HASH :
        {
            ret = get_all_su_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("RSClientAgent::get order mu rule error !");
            }
            break ;
        }
        case MSG_RS_CS_UPDATE_SU_HASH :
        {
            ret = get_su_order_rule(req);
            if(ret < 0)
            {
                LOG_ERROR("RSClientAgent::get order su rule error !");
            }
            break ;
        }

        default :
            LOG_ERROR("RSClientAgent::Undefined cmd type : " + int_to_str(cmd));
            break ;
    }
}

int RSClientAgent::get_all_mu_rule(InReq &req)
{
    std::string out_data;
    MsgHeader header = req.m_msgHeader;
    std::string in_data(req.ioBuf , header.length);
    int ret = -1;
    bool  is_input_error = false;

    uint32_t req_version = 0;
    uint32_t cs_current_version = RulerManager::getInstance()->get_current_version(MU_RULER);

    cstore::pb_MSG_RS_CS_UPDATE_ALL_MU_HASH mu_hash;
    if(!mu_hash.ParseFromString(in_data))
    {
        LOG_ERROR("RSClientAgent::parse protobuf string error ! content "
                + in_data);
        goto ERR;
    }
    req_version = mu_hash.local_version();

    if(req_version < 0 || req_version > cs_current_version)
    {
        LOG_ERROR("RSClientAgent::Invalid Input : current version : " 
                + int_to_str(cs_current_version) + " request version : "
                + int_to_str(req_version));
        is_input_error = true;
        goto ERR ;
    }
    else if(req_version == cs_current_version)
    {
        out_data.clear();
        goto RET ;
    }

    ret = generate_all_hash_and_order_string(MU_RULER , req_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("RSClientAgent::generate all mu hash string error !");
        goto ERR;
    }
    
RET :
    CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK , CS_OK , 
            0 , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    if(is_input_error)
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK , RS_INPUT_ERROR , 
                0 , 0 , out_data);
        return 0;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK , CS_ERROR , 
                0 , 0 , out_data);
        return -1;
    }

    return 0;
}

int RSClientAgent::get_all_su_rule(InReq &req)
{
    std::string out_data;
    MsgHeader header = req.m_msgHeader;
    std::string in_data(req.ioBuf , header.length);
    int ret = -1;
    bool  is_input_error = false;

    uint32_t req_version = 0;
    uint32_t cs_current_version = RulerManager::getInstance()->get_current_version(SU_RULER);

    cstore::pb_MSG_RS_CS_UPDATE_ALL_SU_HASH su_hash;
    if(!su_hash.ParseFromString(in_data))
    {
        LOG_ERROR("RSClientAgent::parse protobuf string error ! content : "
                + in_data);
        goto ERR;
    }
    req_version = su_hash.local_version();

    if(req_version < 0 || req_version > cs_current_version)
    {
        LOG_ERROR("RSClientAgent::Invalid Input : current version : " 
                + int_to_str(cs_current_version) + " request version : "
                + int_to_str(req_version));
        is_input_error = true;
        goto ERR ;
    }

    ret = generate_all_hash_and_order_string(SU_RULER , req_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("RSClientAgent::generate all su hash string error !");
        goto ERR;
    }
    else if(req_version == cs_current_version)
    {
        out_data.clear();
        goto RET ;
    }
    
RET :
    CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_SU_HASH_ACK , CS_OK , 
            0 , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    if(is_input_error)
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK , RS_INPUT_ERROR , 
                0 , 0 , out_data);
        return 0;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK , CS_ERROR , 
                0 , 0 , out_data);
        return -1;
    }

    return 0;
}

int RSClientAgent::get_mu_order_rule(InReq &req)
{
    std::string out_data;
    MsgHeader header = req.m_msgHeader;
    std::string in_data(req.ioBuf , header.length);
    int ret = -1;
    bool  is_input_error = false;

    uint32_t req_version = 0;
    uint32_t cs_current_version = RulerManager::getInstance()->get_current_version(MU_RULER);

    cstore::pb_MSG_RS_CS_UPDATE_MU_HASH mu_hash;
    if(!mu_hash.ParseFromString(in_data))
    {
        LOG_ERROR("RSClientAgent::parse protobuf string error ! content "
                + in_data);
        goto ERR;
    }
    req_version = mu_hash.version();

    if(req_version < 0 || req_version > cs_current_version)
    {
        LOG_ERROR("RSClientAgent::Invalid Input : current version : " 
                + int_to_str(cs_current_version) + " request version : "
                + int_to_str(req_version));
        is_input_error = true;
        goto ERR ;
    }

    ret = generate_plus_order_string(MU_RULER , req_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("RSClientAgent::generate mu order hash string error !");
        goto ERR;
    }
    
    CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_MU_HASH_ACK , CS_OK , 
            0 , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    if(is_input_error)
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_MU_HASH_ACK , RS_INPUT_ERROR , 
                0 , 0 , out_data);
        return 0;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_MU_HASH_ACK , CS_ERROR , 
                0 , 0 , out_data);
        return -1;
    }

    return 0;
}

int RSClientAgent::get_su_order_rule(InReq &req)
{
    std::string out_data;
    MsgHeader header = req.m_msgHeader;
    std::string in_data(req.ioBuf , header.length);
    int ret = -1;
    bool  is_input_error = false;

    uint32_t req_version = 0;
    uint32_t cs_current_version = RulerManager::getInstance()->get_current_version(SU_RULER);

    cstore::pb_MSG_RS_CS_UPDATE_SU_HASH su_hash;
    if(!su_hash.ParseFromString(in_data))
    {
        LOG_ERROR("RSClientAgent::parse protobuf string error ! content : "
                + in_data);
        goto ERR;
    }
    req_version = su_hash.version();

    if(req_version < 0 || req_version > cs_current_version)
    {
        LOG_ERROR("RSClientAgent::Invalid Input : current version : " 
                + int_to_str(cs_current_version) + " request version : "
                + int_to_str(req_version));
        is_input_error = true;
        goto ERR ;
    }

    ret = generate_plus_order_string(SU_RULER , req_version + 1 , out_data);
    if(ret < 0)
    {
        LOG_ERROR("RSClientAgent::generate su order hash string error !");
        goto ERR;
    }
    
    CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_SU_HASH_ACK , CS_OK , 
            0 , 0 , out_data);

    return 0;

ERR :
    out_data.clear();
    if(is_input_error)
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_SU_HASH_ACK , RS_INPUT_ERROR , 
                0 , 0 , out_data);
        return 0;
    }
    else
    {
        CSTCPAgent::reply_request(MSG_RS_CS_UPDATE_SU_HASH_ACK , CS_ERROR , 
                0 , 0 , out_data);
        return -1;
    }

    return 0;
}

int RSClientAgent::generate_all_hash_and_order_string(RULE_TYPE type , uint32_t start_version , std::string &out_data)
{
    out_data.clear();
    RulerManager *rule_ptr = RulerManager::getInstance();
    cstore::pb_MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK  mu_hash_ack;
    cstore::pb_MSG_RS_CS_UPDATE_ALL_SU_HASH_ACK  su_hash_ack;
    cstore::Map_Item                            *map_item_ptr = NULL;
    cstore::Sys_Order                           *sys_order_ptr = NULL;

    uint32_t current_bucket_num = 0;
    uint32_t current_version = 0;
    struct HashRule  hash_rule;
    struct OrderRule order_rule;

    current_bucket_num = rule_ptr->get_bucket_number(type);

    for(uint32_t i = 0 ; i < current_bucket_num ; ++ i)
    {
        if(MU_RULER == type)
        {
            map_item_ptr = mu_hash_ack.add_map_bucket();
        }
        else
        {
            map_item_ptr = su_hash_ack.add_map_bucket();
        }
        memset(&hash_rule , 0 , sizeof(hash_rule));
        if(rule_ptr->get_hash_rule(i , type , hash_rule) < 0)
        {
            LOG_ERROR("RSClientAgent::get hash rule error !");
            return -1;
        }
        
        map_item_ptr->set_bucket_num(hash_rule.bucket_nr);
        for(int j = 0 ; j < MAX_IP_NUM ; ++ j)
        {
            uint32_t ip = hash_rule.ip[j];
            if(INVALID_IP == ip)
                break ;

            map_item_ptr->add_module_ip(ip);
        }
    }

    current_version = rule_ptr->get_current_version(type);
    for(uint32_t i = start_version ; i <= current_version ; ++ i)
    {
        if(MU_RULER == type)
        {
            sys_order_ptr = mu_hash_ack.add_sys_order();
        }
        else
        {
            sys_order_ptr = su_hash_ack.add_sys_order();
        }
        memset(&order_rule , 0 , sizeof(order_rule));
        if(rule_ptr->get_order_rule(i , type , order_rule) < 0)
        {
            LOG_ERROR("RSClientAgent::get order rule error !");
            return -1;
        }

        std::string order_string;
        if(MIGRATION == order_rule.cmd)
        {
            if(generate_migration_string(order_rule.order.migration , order_string) < 0)
            {
                LOG_ERROR("RSClientAgent::generate migration string error !");
                return -1;
            }
            sys_order_ptr->set_order_header(MU_RULER == type ? MSG_MPC_CS_MU_MIGRATE_BUCKET
                    : MSG_MPC_CS_SU_MIGRATE_BUCKET);
        }
        else
        {
            if(generate_extent_string(order_rule.order.extent , order_string) < 0)
            {
                LOG_ERROR("RSClientAgent::generate extent string error !");
                return -1;
            }
            sys_order_ptr->set_order_header(MU_RULER == type ? MSG_MPC_CS_MU_EXTENT_BUCKET
                    : MSG_MPC_CS_SU_EXTENT_BUCKET);
        }
        sys_order_ptr->set_hash_version(order_rule.version);
        sys_order_ptr->set_order_content(order_string);
    }

    bool is_error = false;
    if(MU_RULER == type)
    {
        is_error = ((!mu_hash_ack.IsInitialized()) || (!mu_hash_ack.SerializeToString(&out_data)));
    }
    else
    {
        is_error = ((!su_hash_ack.IsInitialized()) || (!su_hash_ack.SerializeToString(&out_data)));
    }
    if(is_error)
    {
        LOG_ERROR("RSClientAgent::protobuf initialized error !");
        return -1;
    }

    if(MU_RULER == type)
    {
        LOG_EVENT("mu hash and order info : " + mu_hash_ack.DebugString());
    }
    else
    {
        LOG_EVENT("su hash and order info : " + su_hash_ack.DebugString());
    }

    return 0;
}

int RSClientAgent::generate_plus_order_string(RULE_TYPE type , uint32_t start_version , std::string &out_data)
{
    out_data.clear();
    struct OrderRule  order_rule;
    uint32_t current_version = RulerManager::getInstance()->get_current_version(type);

    cstore::pb_MSG_SYS_RS_UPDATE_MU_HASH_ACK  mu_order_ack;
    cstore::pb_MSG_SYS_RS_UPDATE_SU_HASH_ACK  su_order_ack;
    cstore::Sys_Order                        *sys_order_ptr = NULL;

    for(uint32_t i = start_version ; i <= current_version ; ++ i)
    {
        if(MU_RULER == type)
        {
            sys_order_ptr = mu_order_ack.add_sys_order();
        }
        else
        {
            sys_order_ptr = su_order_ack.add_sys_order();
        }
        memset(&order_rule , 0 , sizeof(order_rule));
        if(RulerManager::getInstance()->get_order_rule(i , type , order_rule) < 0)
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

int RSClientAgent::generate_migration_string(const Migration_Info &info , std::string &out_data)
{
    cstore::pb_MSG_MPC_CS_SU_MIGRATE_BUCKET  migration_pb;

    migration_pb.set_source_ip(info.src_ip);
    migration_pb.set_destination_ip(info.dest_ip);
    migration_pb.set_bucket_num(info.bucket_nr);

    if((!migration_pb.IsInitialized()) || (!migration_pb.SerializeToString(&out_data)))
    {
        LOG_ERROR("SystemAgent::protobuf initialized error when migration string !");
        return -1;
    }

    return 0;
}

int RSClientAgent::generate_extent_string(const Extent_Info &info , std::string &out_data)
{
    cstore::pb_MSG_MPC_CS_SU_EXTENT_BUCKET extent_pb;

    extent_pb.set_new_mod(info.new_mod);

    if((!extent_pb.IsInitialized()) || (!extent_pb.SerializeToString(&out_data)))
    {
        LOG_ERROR("SystemAgent::protobuf initialized error when extent string !");
        return -1;
    }

    return 0;
}
