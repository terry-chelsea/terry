/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  main函数，提供选项
 *
 *        Version:  1.0
 *        Created:  11/05/12 14:57:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  fengyuatad@126.com(terry)
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "CSConfiguration.h"
#include "RunControl.h"
#include "Log_warp.h"
#include "Lib_tool.h"

#define DEFAULT_CONFIG_FILE_NAME "./CSConfig.xml"

DevLog* g_pDevLog;

void Usage(const string &proc_name)
{
    fprintf(stderr , "Uasge : %s [-f config_file_path]\n" 
            , proc_name.c_str());
}

void sig_int(int signo)
{
    if(signo == SIGINT)
    {
        std::cerr<<"Downing ..."<<std::endl;
        std::cerr<<"Reason : "<<get_dead_line()<<std::endl;
        exit(-1);
    }
}

int set_coredump()
{
    struct rlimit limit;
    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    if(setrlimit(RLIMIT_CORE , &limit) < 0)
    {
        LOG_ERROR("Main::setrlimit error !");
        return -1;
    }
    return 0;
}

int main(int argc , char *argv[])
{
    std::string config_file = DEFAULT_CONFIG_FILE_NAME ;
    int opt = 0;

    //根据选项启动，选项为-f，后跟参数是启动的配置文件路径0
    while((opt = getopt(argc , argv , "f:st")) != -1)
    {
        switch(opt)
        {
            case 'f' :
                config_file = optarg;
                break ;
            case 's' :
                CSConfiguration::getInstance()->start_derict = true;
                break;
            case 't' :
                CSConfiguration::getInstance()->test_register = true;
                break;
            case '?' :
                Usage(argv[0]);
                exit(-1);
                break ;
            default :
                Usage(argv[0]);
                exit(-1);
                break;
        }
    }

    signal(SIGINT , sig_int);
    g_pDevLog = new DevLog();
    
    //初始化读取配置文件的信息
    int ret = CSConfiguration::getInstance()->configWithXML(config_file);
    if(ret < 0)
    {
        std::cerr<<"Parse configuer file error !"<<std::endl;
        exit(-1);
    }
    else
    {
        //打印配置文件的信息
        CSConfiguration::getInstance()->showConfig();
    }
    if(set_coredump() < 0)
    {
        LOG_ERROR("set coredump file size error !");
        exit(-1);
    }

    if(RulerManager::getInstance()->init_rule() < 0)
    {
        LOG_ERROR("Init ruler in RulerManager error !");
        exit(-1);
    }

    ret = RunControl::getInstance()->prepare_run();
    if(ret < 0)
    {
        LOG_ERROR("CS & RS Initialized Failed !");
        exit(-1);
    }

    //完成真正的启动，开始监听
    ret = RunControl::getInstance()->run();
    if(ret < 0)
    {
        LOG_ERROR("CS & RS Execuate Failed !");
        exit(-1);
    }

//    delete RunControl::getInstance();
    
    return 0;
}

