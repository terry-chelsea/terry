/*
 * =====================================================================================
 *
 *       Filename:  work_func.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/26/13 11:48:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_WORK_FUNC_MAP_H_
#define _H_WORK_FUNC_MAP_H_

#include <map>
#include "Header.h"

struct Client_Req
{
    Header head;
    char *load;
    int fd;
};

const unsigned short ERROR_INFO_LENGTH = 32;
typedef int (*work_func)(const struct Client_Req &);

struct Work_info
{
    work_func func;
    char error[ERROR_INFO_LENGTH];
};
typedef std::map<unsigned int , struct Work_info> Work_Func_map;

void register_function(Work_Func_map &mp , unsigned int , work_func func , const char *error);

work_func get_function(Work_Func_map mp , unsigned int , char *&);

void display_functions(Work_Func_map mp);

#endif
