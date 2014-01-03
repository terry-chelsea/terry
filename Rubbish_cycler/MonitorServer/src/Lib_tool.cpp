// =====================================================================================
// 
//       Filename:  Tool.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/12 19:54:38
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  litongxing (), litongxing2046@163.com
//        Company:  NDSL
// 
// =====================================================================================
#include "Lib_tool.h"

unsigned int str_to_int(const std::string & str_value)
{
    if(str_value == "")
        return 0;
    std::stringstream sst;
    unsigned int int_value;
    sst << str_value;
    sst >> int_value;
    return int_value;
}

std::string int_to_str(unsigned int int_value)
{
    std::stringstream sst;
    std::string str_value;
    sst << int_value;
    sst >> str_value;
    return str_value;
}

std::string ushort_to_str(unsigned short short_value)
{
    std::stringstream sst;
    std::string str_value;
    sst << short_value;
    sst >> str_value;
    return str_value;
}
unsigned short str_to_ushort(const std::string& str_value)
{
    if(str_value == "")
        return 0;
    std::stringstream sst;
    unsigned short short_value;
    sst << str_value;
    sst >> short_value;
    return short_value;

}

uint32_t cs_pow(uint16_t value , uint8_t base)
{
    if(value < 0)
        return 0;

    uint32_t ret = 1;
    for(int i = 0 ; i < value ; ++ i)
        ret *= base;

    return ret;
}

int cs_log(uint32_t value , uint8_t base)
{
    uint32_t pow_value = 0;

    int i = 0;
    for(i = 0 ; pow_value <= value ; ++ i)
    {
        pow_value = cs_pow(i , base);
        if(value == pow_value)
            break;
    }

    if(pow_value > value)
        return -1;
    else
        return i;
}

int int_to_string_ip(uint32_t ip , std::string &ip_str)
{
    char ip_temp[32];
    if(inet_ntop(AF_INET , &ip , ip_temp , sizeof(ip_temp)) == NULL)
    {
        std::cerr<<"Inet_ntop error : "<<strerror(errno)<<std::endl;
        return -1;
    }

    ip_str.assign(ip_temp);
    return 0;
}

int string_to_int_ip(const std::string &ip_str , uint32_t &ip)
{
    const char *ip_temp = ip_str.c_str();
    if(inet_pton(AF_INET , ip_temp , &ip) < 0)
    {
        std::cerr<<"Inet_pton error : "<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int get_last_ip_num(const std::string &ip_str)
{
    //找到最后一个字符的指针，然后依次向前进行计算
    const char *ip_str_end = ip_str.c_str() + ip_str.size() - 1;
    const char *ip_temp = ip_str_end;
    char value = 0;
    int ret_value = 0;
    int i = 0;

    while(ip_temp != ip_str.c_str() && *ip_temp != '.')
    {
        //记住！要减去一个字符0！！！
        value = *ip_temp - '0';

        ret_value += cs_pow(i , 10) * value;
        -- ip_temp;
        ++ i;
    }
    
    if(ret_value <= 0 || ret_value >= 225)
    {
        std::cerr<<"Invalid ip adderss : "<<ip_str<<std::endl;
        ret_value = -1;
    }
        
    return ret_value;
}

void append_new_ip(std::string &ip_str , int last_num)
{
    const char *ip_str_end = ip_str.c_str() + ip_str.size() - 1;
    const char *ip_temp = ip_str_end;
    
    while(ip_temp != ip_str.c_str() && *ip_temp != '.')
    {
        -- ip_temp;
    }
    ++ ip_temp;

    ip_str.assign(ip_str.c_str() , ip_temp - ip_str.c_str());
    
    ip_str += int_to_str(last_num);
}

void set_dead_line(const std::string & line)
{
    dead_line.assign(line);
    die();
}

std::string get_dead_line()
{
    if(dead_line.empty())
        dead_line.assign("User Kill me...");
    return dead_line;
}

void die()
{
    kill(getpid() , SIGINT);
}

std::string string_ip(uint32_t ip)
{
    std::string ret;
    if(int_to_string_ip(ip , ret) < 0)
    {
        std::cerr<<"int to string ip error !"<<std::endl;
        ret = " ip error !";
    }

    return ret;
}
