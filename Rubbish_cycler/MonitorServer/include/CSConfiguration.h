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

class CSConfiguration: public Singleton<CSConfiguration> {
   friend class Singleton<CSConfiguration>;

    public:
        bool     Is_CS;
        bool     Is_RS;

        uint16_t mu_port;             //MU监听端口
        uint16_t su_port;             //SU监听端口
        uint32_t rs_port;             //RS监听端口
        uint16_t cs_listen_mpc_port;  //监听MPC的端口
        uint16_t mpc_to_port;         //向MPC发送数据的端口
        uint16_t cs_listen_hb_port;   //监听心跳的端口
        uint16_t cs_listen_rs_port;   //监听RS拉取规则的端口
        
        std::string hostip;           //本机的IP
        std::string dsn;              //数据源
        
        //只有本模块具有RS功能的时候才使用这个套接口
        uint16_t rs_listen_sys_port;             //RS监听客户端的套接口
        uint16_t rs_cs_port;                     //RS向CS拉取规则的端口
        uint16_t rs_listen_cs_port;              //RS监听CS更新规则通知的端口
        uint16_t rs_cs_hb_port;                  //CS心跳监听端口
        std::string  rs_cs_ip;
        bool m_direct_reply;
        uint16_t m_rs_heartbeat_interval;

        LOG_OUT_TYPE log_type;
        uint16_t thread_num;

        uint16_t check_heartbeat_interval;
        uint16_t report_MPC_interval;
        uint16_t heartbeat_timeout;

        //MPC的IP通过程序中MPC的连接获得，每次连接都更新
        std::string mpc_ip;
    public:
        
        std::string getIPaddress();

        int configWithXML(const std::string &configFileName);

        void showConfig();
    private :

        CSConfiguration();
        ~CSConfiguration();
};

#endif  // _CSCONFIGURATION_H_

