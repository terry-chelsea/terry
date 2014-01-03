// =====================================================================================
// 
//       Filename:  Log_warp.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/08/12 20:28:17
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  litongxing (), litongxing2046@163.com
//        Company:  NDSL
// 
// =====================================================================================
#ifndef _LOG_WARP_H_
#define _LOG_WARP_H_

#include "../common/DevLog/DevLog.h"

#define LOG_INIT(info) dev_log_init_info(info)
#define LOG(str) dev_log(str, __FILE__, __FUNCTION__, __LINE__)
#define LOG_EVENT(str) dev_log_event(str, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR(str) dev_log_error(str, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR_NUM(errnum,str) dev_log_error_num(errnum, str, __FILE__, __FUNCTION__, __LINE__);
#define LOG_FATAL(str) dev_log_fatal(str,__FILE__,__FUNCTION__,__LINE__)
#define LOG_SYSCALL_ERROR(str) dev_log_syscall_error(str, __FILE__, __FUNCTION__, __LINE__)
//#define LOG_ERRNUM() 

extern void dev_log(const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);

extern void dev_log_event(const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);

extern void dev_log_error(const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);


extern void dev_log_error_num(const int errnum, \
                    const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);


extern void dev_log_fatal(const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);

extern void dev_log_syscall_error(const std::string& msg,\
                    const char *str_file,\
                    const char *str_function,\
                    const int iLine);


extern void dev_log_init_info(const std::string &msg);

#endif

