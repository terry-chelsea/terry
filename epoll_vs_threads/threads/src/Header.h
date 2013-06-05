/*
 * =====================================================================================
 *
 *       Filename:  Header.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/13 15:09:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_HEADER_H_
#define _H_HEADER_H_

#ifndef uint32_t

#include <stdio.h>
#include <bits/types.h>
typedef __uint32_t uint32_t;
#endif

typedef struct MsgHeader
{
    uint32_t cmd;
    uint32_t length;
    uint32_t error;
    uint32_t para1;
    uint32_t para2;
}Header;

typedef int (*client_work)(int);

/*  
#ifdef __cplusplus
extern "C"
{
#endif
*/

extern int start_mutil_thread_server(char * , unsigned short , client_work);

//return -1 for failed and return 0 for peer close and postive for read enough data , means read len to buf
extern int readn_blocking(int fd , char *buf , size_t len);
//same to read function...
extern int writen_blocking(int fd , char *buf , size_t len);

//return NULL means error or return address of ip , need free those memory after use...
extern char * get_peer_info(int fd , unsigned short *port);

    /*  
#ifdef __cplusplus
}

#endif
*/


#endif
