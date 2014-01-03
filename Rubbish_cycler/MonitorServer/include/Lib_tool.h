// =====================================================================================
// 
//       Filename:  Tool.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/09/12 19:44:12
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  litongxing (), litongxing2046@163.com
//        Company:  NDSL
// 
// =====================================================================================
#ifndef __TOOL__H_
#define __TOOL__H_

#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstdlib>
#include <signal.h>

static std::string dead_line;

extern unsigned int str_to_int(const std::string& string_value);
extern std::string int_to_str(unsigned int int_value);
extern std::string ushort_to_str(unsigned short short_value);
extern unsigned short str_to_ushort(const std::string& string_value);
extern uint32_t cs_pow(uint16_t value , uint8_t base = 2);
extern int cs_log(uint32_t value , uint8_t base = 2);

extern int int_to_string_ip(uint32_t ip, std::string &ip_str);
extern int string_to_int_ip(const std::string &ip_str , uint32_t &ip);

extern int get_last_ip_num(const std::string &ip_str);

extern void append_new_ip(std::string &ip_str , int last_num);

extern void set_dead_line(const std::string &);

void die();

extern std::string get_dead_line();

#endif
