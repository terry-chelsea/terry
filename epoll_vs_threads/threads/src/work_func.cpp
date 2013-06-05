/*
 * =====================================================================================
 *
 *       Filename:  work_func.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/26/13 11:58:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "work_func.h"
#include "log.h"
#include <cstring>
#include <utility>

void register_function(Work_Func_map &mp , unsigned int cmd , work_func func , const char *msg)
{
    int len = strlen(msg) + 1;
    struct Work_info info;
    info.func = func;
    if(len > ERROR_INFO_LENGTH)
    {
        memcpy(info.error , msg , ERROR_INFO_LENGTH);
        (info.error)[ERROR_INFO_LENGTH] = '\0';
    }
    else
        memcpy(info.error , msg , len);

    std::pair<Work_Func_map::iterator , bool>ret = mp.insert(std::make_pair(cmd , info));
    if(!(ret.second))
        LOG_WARNING("command %u has been register to this map , error message : %s" , cmd , msg);
}

work_func get_function(Work_Func_map mp , unsigned cmd , char *&err)
{
    Work_Func_map::iterator it = mp.find(cmd);
    if(it != mp.end())
    {
        err = (it->second).error;
        return (it->second).func;
    }
    else
    {
        LOG_ERROR("Can not find command %d function in map" , cmd);
        return NULL;
    }
}

void display_functions(Work_Func_map mp)
{
    struct Work_info info;
    Work_Func_map::iterator it = mp.begin();
    if(it == mp.end())
        printf("Functions map is empty !\n");
    while(it != mp.end())
    {
        info = it->second;
        printf("command : %u and error code %s\n" , it->first , info.error);

        ++ it;
    }
}
