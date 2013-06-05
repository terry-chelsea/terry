/*
 * =====================================================================================
 *
 *       Filename:  Header.h
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  2010年04月07日 17时16分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), duanhancong@uestc.edu.cn
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef _BASE_HEADER_H_
#define _BASE_HEADER_H_
//#include "Error.h"
//#include "SocketAddress.h"
//#define FAILED -1
//#define SUCCESSFUL 0
const int CONNECTING = 0;
const int CONNECTED = 1;
const int CONNECTFAILED = -1;
const int MAXRECONNECTTIMES = 3;
#define IPV4 4
#define IPV6 6
#define IPV4ANYADDR "0.0.0"
#define IPV6ANYADDR "::1"
//const SocketAddress InitAddr("0", 0);

const unsigned int HEADER_SIZE = sizeof(int) * 5;
struct MsgHeader
{
    unsigned int cmd;	// 消息类型
    unsigned int length; //数据消息长度
    unsigned int error;
    unsigned int para1;
    unsigned int para2;
};
#endif
