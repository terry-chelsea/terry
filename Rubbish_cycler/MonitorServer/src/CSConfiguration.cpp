/*
 * =====================================================================================
 *
 *       Filename:  CSConfiguration.cpp
 *
 *    Description:  CS配置注册表
 *
 *        Version:  1.0
 *        Created:  2011-05-03 11:58:44
 *  Last Modified:  2011-08-02 14:52:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NCSL UESTC
 *
 * =====================================================================================
 */

#include "CSConfiguration.h"

#include <cstdlib>

#include <string>
#include <iostream>
#include <sys/types.h>
#include <inttypes.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "../common/util/util.h"
#include "Lib_tool.h"
#include "config_path.h"

#define ADDR_ANY "0"

CSConfiguration::CSConfiguration()
    :db_url()
{
    mu_port = 0;             //MU监听端口
    su_port = 0;             //SU监听端口
    cs_listen_mpc_port = 0;  //监听MPC的端口
    cs_listen_hb_port = 0;   //监听心跳的端口
    rule_init_port = 0;

    //只有本模块具有RS功能的时候才使用这个套接口
    rs_listen_sys_port = 0;             //RS监听客户端的套接口

    thread_num = 0;

    check_heartbeat_interval = 0;
    heartbeat_timeout = 0;

    migration_retry_time = 10;

    mu_number = 0;
    su_number = 0;

    start_derict = false;
    test_register = false;
}

CSConfiguration::~CSConfiguration()
{
}

std::string CSConfiguration::getIPaddress()
{
    int sfd, intr;
    struct ifreq buf[16];
    struct ifconf ifc;
    sfd = socket (AF_INET, SOCK_DGRAM, 0);
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;
    if(ioctl(sfd, SIOCGIFCONF, (char *)&ifc))
    {
        std::cout << "get IP error" << std::endl;
        exit(0);
    }
    intr = ifc.ifc_len / sizeof(struct ifreq);
    while (intr-- > 0 && ioctl(sfd, SIOCGIFADDR, (char *)&buf[intr]));
    close(sfd);
    char* ips = inet_ntoa(((struct sockaddr_in*)(&buf[intr].ifr_addr))-> sin_addr);
    std::string stIP(ips);

    return stIP;
}

int CSConfiguration::configWithXML(const std::string &conf_file_name)
{
#ifndef CSN 
    XMLConfig *xml = new XMLConfig(conf_file_name.c_str());
#else
    XMLConfig *xml = new XMLConfig();
    xml->loadMemory(conf_file_name.c_str() , conf_file_name.size());
#endif

    std::string value;
    int num = 0;

    this->hostip = ADDR_ANY ;
    
#ifndef CSN
    const char **config = config_normal;
#else 
    const char **config = config_auto;
#endif
//    this->hostip = getIPaddress();
    //判断当前模块是否是CS模块

    //日志输出级别
    if(xml->getFirstNodeValue(config[LOG_TYPE] , value) != -1)
    {
        int type = atoi(value.c_str());
        if(0 == type)
        {
            this->log_type = OUT_SCR;
        }
        else if(1 == type)
        {
            this->log_type = OUT_FILE;
        }
        else if(2 == type)
        {
            this->log_type = OUT_BOTH;
        }
        else
        {
            std::cerr<<"Undefined Log type !"<<std::endl;
            this->log_type = DEFAULT_LOG_TYPE;
        }
    }
    else
    {
        std::cerr<<"parse Log type from configuer file error !"<<std::endl;
        return -1;
    }
    
    //MU监听端口
    if(xml->getFirstNodeValue(config[MU_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Port/MU_port error");
        goto ERR;
    }
    this->mu_port = (uint16_t)(atoi(value.c_str()));
    
    //SU监听的端口    
    if(xml->getFirstNodeValue(config[SU_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Port/SU_port error");
        goto ERR;
    }
    this->su_port = (uint16_t)(atoi(value.c_str()));

    if(xml->getFirstNodeValue(config[MPC_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Port/MPC_listen_port error");
        goto ERR;
    }
    
    
    this->cs_listen_mpc_port = (uint16_t)(atoi(value.c_str()));

    //监听各种心跳的套接口
    if(xml->getFirstNodeValue(config[HB_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse MU_CS_PORT error");
        goto ERR;
    }
    this->cs_listen_hb_port = (uint16_t)(atoi(value.c_str()));

    if(xml->getFirstNodeValue(config[INIT_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse SU_RS_PORT1 error");
        goto ERR;
    }
    this->rule_init_port = (uint16_t)(atoi(value.c_str()));

    if(xml->getFirstNodeValue(config[RS_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse MU_RS_PORT error");
        goto ERR;
    }
    this->rs_listen_sys_port = (uint16_t)(atoi(value.c_str()));

    if(xml->getFirstNodeValue(config[THREAD_NUM] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Thread/number error");
        goto ERR;
    }
    this->thread_num = atoi(value.c_str());
    if(this->thread_num < 0)
    {
        this->thread_num = DEFAULT_THREAD_NUMBER;
    }

    if(xml->getFirstNodeValue(config[TIMER_HB] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Time/Check_HB error");
        goto ERR;
    }
    this->check_heartbeat_interval = atoi(value.c_str());
    if(this->check_heartbeat_interval < 0)
    {
        this->check_heartbeat_interval = DEFAULT_CHECK_TIMEOUT;
    }

    if(xml->getFirstNodeValue(config[TIMEOUT_HB] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Time/HB_timeout error");
        goto ERR;
    }
    this->heartbeat_timeout = atoi(value.c_str());
    if(this->heartbeat_timeout < 0)
    {
        this->heartbeat_timeout = DEFAULT_HB_TIMEOUT * this->check_heartbeat_interval;
    }

    if(init_db_info(xml) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse database info error !");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[MIGRATION_RETRY] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Migration/Retry error");
        goto ERR;
    }
    this->migration_retry_time = atoi(value.c_str());
    if(this->migration_retry_time < 0)
    {
        this->migration_retry_time = DEFAULT_RETRY_TIMES;
    }

    if(xml->getFirstNodeValue(config[MU_NODES] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/MU error");
        goto ERR;
    }
    if((num = atoi(value.c_str())) < 0)
    {
        LOG_ERROR("Initialize MU Node number MUST more than 0!");
        goto ERR;
    }
    this->mu_number = num;
    this->mu_flag = (this->mu_number == 0);

    if(xml->getFirstNodeValue(config[SU_NODES] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/SU error");
        goto ERR;
    }
    if((num = atoi(value.c_str())) < 0)
    {
        LOG_ERROR("Initialize SU Node number MUST more than 0!");
        goto ERR;
    }
    this->su_number = num;
    this->su_flag = (this->su_number == 0);

    if(xml->getFirstNodeValue(config[MU_MOD] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/MU_mod error");
        goto ERR;
    }
    if((this->mu_init_mod = atoi(value.c_str())) <= 0)
        this->mu_init_mod = DEFAULT_MU_MOD;

    if(xml->getFirstNodeValue(config[SU_MOD] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/SU_mod error");
        goto ERR;
    }
    if((this->su_init_mod = atoi(value.c_str())) <= 0)
        this->su_init_mod = DEFAULT_SU_MOD;

    if(xml->getFirstNodeValue(config[MU_DUP] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/MU_dup error");
        goto ERR;
    }
    if((this->mu_init_dup = atoi(value.c_str())) <= 0 || 
            (this->mu_init_dup = atoi(value.c_str())) > 5)
        this->mu_init_dup = DEFAULT_MU_DUP;

    if(xml->getFirstNodeValue(config[SU_DUP] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::parse /CS/Node/SU_dup error");
        goto ERR;
    }
    if((this->su_init_dup = atoi(value.c_str())) <= 0 || 
            (this->su_init_dup = atoi(value.c_str())) > 5)
        this->su_init_dup = DEFAULT_SU_DUP;

    return 0;

ERR:
    return -1;
}

int CSConfiguration::init_db_info(XMLConfig *xml)
{
#ifdef CSN
    const char **config = config_auto;
#else
    const char **config = config_normal;
#endif
    std::string db_name;
    std::string db_user;
    std::string db_passwd;
    std::string db_ip;
    std::string value;
    uint16_t db_port;
    if(xml->getFirstNodeValue(config[DB_NAME] , db_name) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/Db error");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[DB_USER] , db_user) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/User error");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[DB_PASSWD] , db_passwd) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/Passwd error ");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[DB_IP] , db_ip) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/IP error ");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[DB_PORT] , value) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/Port error ");
        goto ERR;
    }
    db_port = (uint16_t)(atoi(value.c_str()));

    generate_db_url(db_name , db_user , db_passwd , db_ip , db_port);

    if(xml->getFirstNodeValue(config[MU_HASH] , this->mu_rule_table) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/MU_hash error !");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[MU_ORDER] , this->mu_order_table) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/MU_order error !");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[SU_HASH] , this->su_rule_table) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/SU_hash error !");
        goto ERR;
    }
    
    if(xml->getFirstNodeValue(config[SU_ORDER] , this->su_order_table) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/SU_order error !");
        goto ERR;
    }

    if(xml->getFirstNodeValue(config[HISTORY_TABLE] , this->history_table) < 0)
    {
        LOG_ERROR("CSConfiguration::Parse /CS/DB/History error !");
        goto ERR;
    }

    return 0;
ERR:
    return 1;
}


void CSConfiguration::showConfig()
{
    std::cerr<<"---------------configuration Info------------"<<std::endl;
    std::cerr<<"Host IP : "<<hostip<<std::endl;
    std::cerr<<"Listen Heartbeat Port : "<<this->cs_listen_hb_port<<std::endl;
    std::cerr<<"Listen MPC Port : "<<this->cs_listen_mpc_port<<std::endl;
    std::cerr<<"MU Listen Port : "<<this->mu_port<<std::endl;
    std::cerr<<"SU Listen Port : "<<this->su_port<<std::endl;
    std::cerr<<"Listen Node Register Port : "<<this->rule_init_port<<std::endl;
    std::cerr<<"Listen Rule Request Port : "<<this->rs_listen_sys_port<<std::endl;
    std::cerr<<"Thread Number : "<<this->thread_num<<std::endl;
    std::cerr<<"Heartbeat Timeout : "<<this->heartbeat_timeout<<"s"<<std::endl;
    std::cerr<<"Check Heartbeat Interval : "<<this->check_heartbeat_interval<<"s"<<std::endl;
    std::cerr<<"database Infonation : "<<this->db_url<<std::endl;
    std::cerr<<"database tables : "<<this->mu_rule_table<<" ; "<<this->su_rule_table<<" ; "<<this->mu_order_table<<" ; "<<this->su_order_table<<" ; "<<this->history_table<<std::endl;

    std::cerr<<"Migration retry times : "<<this->migration_retry_time<<std::endl;
    std::cerr<<"MU Node Number : "<<this->mu_number<<" mod : "<<mu_init_mod<<" dup : "<<mu_init_dup<<std::endl;
    std::cerr<<"SU Node Number : "<<this->su_number<<" mod : "<<su_init_mod<<" dup : "<<su_init_dup<<std::endl;
    std::cerr<<"Log type : "<<this->log_type<<std::endl;

    std::cerr<<"--------------------------------------------"<<std::endl;
}

void CSConfiguration::generate_db_url(const std::string &name , const std::string &user , 
        const std::string &pwd , const std::string &ip , const uint16_t port)
{
    //使用libzdb，mysql的URL的格式如下：
    //mysql://user:passwd@db_server_ip:db_server_port/db_name
    this->db_url.assign("mysql://");
    this->db_url += user;
    this->db_url += ":";
    this->db_url += pwd;
    this->db_url += "@";
    this->db_url += ip;
    this->db_url += ":";
    this->db_url += int_to_str(port);
    this->db_url += "/";
    this->db_url += name;
}
