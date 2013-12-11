/*
 * =====================================================================================
 *
 *       Filename:  Tool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:43:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_TOOL_H_
#define _H_TERRY_TOOL_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include "Log/Debug.h"

typedef enum
{
    SET_BLOCK = 0,
    SET_NONBLOCK = 1
}SET_TYPE;

extern int set_nonblock(int fd , SET_TYPE type);

extern int read_from_thread_blocking(int fd , int *ret);

extern int read_with_blocking(int fd , char *buf , int *ret_length);

extern int write_with_blocking(int fd , void *data , int length);

extern int write_to_thread_blocking(int fd , void *data , int length);

#endif
