/*
 * =====================================================================================
 *
 *       Filename:  Debug.h
 *
 *    Description:  use those function debug printf some useful imformations...
 *
 *        Version:  1.0
 *        Created:  03/28/12 22:37:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (terry),fengyuatad@126.com 
 *        Company:  
 *
 * =====================================================================================
 */
    
#ifndef _H_MY_DEBUG_H_
#define _H_MY_DEBUG_H_

#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define MY_FILE_NAME_LENGTH 256                 //日志文件的最大长度
#define MY_MAX_LINE         1024
#define COUT_ONE_LINE       16
#define ADDR_LENGTH         8                   //IP地址的最打字节数
#define BIN_CODE_LENGTH     (COUT_ONE_LINE * 2 + COUT_ONE_LINE / 2)


typedef struct 
{
    int  isPrintfTTY;
    FILE *fp;
}MYDEBUG;

MYDEBUG *beginDebug(char * , char *);

void endDebug(MYDEBUG **);

int mySprintf(char * , int , char * , ...);

int printDebugInfo(MYDEBUG * , char * , ...);

void getTime(char * , int );

void binDebugOutput(MYDEBUG * , void * , int );

#endif
