// =====================================================================================
// 
//       Filename:  Log_warp.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/12 11:23:32
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  litongxing (), litongxing2046@163.com
//        Company:  NDSL
// 
// =====================================================================================
#include <string>
#include <cstring>
#include <errno.h>
#include "Lib_tool.h"
#include "Log_warp.h"
#include "CSConfiguration.h"

extern DevLog* g_pDevLog;

#define DEBUG

void dev_log(
        const std::string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LDEBUG , CSConfiguration::getInstance()->log_type , msg + \
                " [in: " + std::string(strFile) + \
                " in fun: "+std::string(strFunction)+ \
                " on line: " +std::string(int_to_str(Line)) + "]");
#endif
}

void dev_log_event(
        const std:: string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LEVENT , CSConfiguration::getInstance()->log_type , msg + \
                " [File: " + std::string(strFile) + \
                " Fun: "+std::string(strFunction)+ \
                " Line: " +std::string(int_to_str(Line)) + "]");
#endif
}

void dev_log_error(
        const std::string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LERROR , CSConfiguration::getInstance()->log_type , msg + \
                " [File: " + std::string(strFile) + \
                " Fun: "+std::string(strFunction)+ \
                " Line: " +std::string(int_to_str(Line)) + "]");
#endif
}

void dev_log_error_num(
        const int errnum,
        const std::string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LERROR , CSConfiguration::getInstance()->log_type , msg + \
                " [File: " + std::string(strFile) + \
                " Fun: "+std::string(strFunction)+ \
                " Line: " +std::string(int_to_str(Line)) + "]");
#endif
}


void dev_log_fatal(
        const std::string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LFATAL , CSConfiguration::getInstance()->log_type , msg + \
                " [File: " + std::string(strFile) + \
                " Fun: "+std::string(strFunction)+ \
                " Line: " +std::string(int_to_str(Line)) + "]");
#endif
}

void dev_log_syscall_error(
        const std::string& msg,
        const char *strFile,
        const char *strFunction,
        const int Line)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LFATAL , CSConfiguration::getInstance()->log_type , msg + \
                " [File: " + std::string(strFile) + \
                " errnoInfo: "+std::string(std::strerror(errno))+\
                " Fun: " +std::string(strFunction)+ \
                " Line: " +std::string(int_to_str(Line)) + "]");
#endif
}

void dev_log_init_info(const std::string &msg)
{
#ifdef DEBUG
    g_pDevLog->writeLog(LEVENT , CSConfiguration::getInstance()->log_type , msg);
#endif
}



