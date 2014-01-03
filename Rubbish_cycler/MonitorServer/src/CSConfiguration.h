/*
 * =====================================================================================
 *
 *       Filename:  CSConfiguration.h
 *
 *    Description:  CS配置注册表
 *
 *        Version:  1.0
 *        Created:  2011-05-03 11:37:06
 *  Last Modified:  2011-08-02 14:50:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NCSL UESTC
 *
 * =====================================================================================
 */

#ifndef _CSCONFIGURATION_H_
#define _CSCONFIGURATION_H_

#include <string>

#include <sys/types.h>
#include <inttypes.h>

#include "../common/sys/Singleton.h"
#include "../common/xml/XMLConfig.h"

#include "Log_warp.h"

#define DEFAULT_LOG_TYPE OUT_BOTH
#define DEFAULT_RETRY_TIMES 10
#define DEFAULT_THREAD_NUMBER 2
#define DEFAULT_CHECK_TIMEOUT 10
#define DEFAULT_HB_TIMEOUT    3

#define DEFAULT_MU_DUP      3
#define DEFAULT_SU_DUP      2
#define DEFAULT_SU_MOD      3
#define DEFAULT_MU_MOD      3

class CSConfiguration: public Singleton<CSConfiguration> {
   friend class Singleton<CSConfiguration>;

    public:
        uint16_t mu_port;             //MU监听端口
        uint16_t su_port;             //SU监听端口
        uint16_t cs_listen_mpc_port;  //监听MPC的端口
        uint16_t cs_listen_hb_port;   //监听心跳的端口
        uint16_t rule_init_port;
        uint16_t rs_listen_sys_port;             //RS监听客户端的套接口
        uint16_t mu_rc_port;
        uint32_t su_rc_port;
        
        std::string hostip;           //本机的IP
        
        LOG_OUT_TYPE log_type;
        int thread_num;

        int check_heartbeat_interval;
        int heartbeat_timeout;

        int migration_retry_time;

        std::string db_url;
        std::string mu_rule_table;
        std::string su_rule_table;
        std::string su_order_table;
        std::string mu_order_table;
        std::string history_table;

        uint32_t mu_number;
        uint32_t su_number;

        int mu_init_mod;
        int mu_init_dup;
        int su_init_mod;
        int su_init_dup;

        bool start_derict;
        bool test_register;

        bool su_flag;
        bool mu_flag;
    public:
        
        std::string getIPaddress();

        int configWithXML(const std::string &configFileName);

        void showConfig();
    private :

        CSConfiguration();
        ~CSConfiguration();

        int init_db_info(XMLConfig *);
        void generate_db_url(const std::string & , const std::string & , const std::string & , const std::string & , const uint16_t );
};

#endif  // _CSCONFIGURATION_H_

