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

#define DEBUG
#define DEBUG_LEVEL 2

/* 使用方式 ：首先如果需要调试需要定义DEBUG宏定义
 * 使用START_DEBUG宏定义初始化启动调试，参数是特定的文件名和日志类型
 * 使用结束需要调用FINISH_DEBUG释放内存 
 * 如果没有启动调试就调用接口，会打印一次Hint,以后也不会有任何输出
 * 可以使用的接口有下面的宏定义
 * 使用方式和标准的格式化输出一致
 */

#define MY_FILE_NAME_LENGTH 256                 //日志文件的最大长度
#define MY_MAX_LINE         512                 //一行缓冲区的最大长度
#define COUT_ONE_LINE       16                  //打印二进制的时候一行的字符数
#define ADDR_LENGTH         16                   //IP地址的最打字节数
#define BIN_CODE_LENGTH     (COUT_ONE_LINE * 2 + COUT_ONE_LINE / 2)

#define START_DEBUG(app_name , type) do{ \
    beginDebug(app_name , type); \
    printDebugInfoWithoutTime("\n"); \
}while(0)

#define FINISH_DEBUG() do{ \
    printDebugInfoWithoutTime("\n"); \
    endDebug(); \
}while(0)

#if(DEBUG_LEVEL > 0)

#define DEBUG_BIN_CODE(struct_ptr , s_size)  
#define DEBUG_BIN_CODE_TIME(struct_ptr , s_size)  

#else

#define DEBUG_BIN_CODE(struct_ptr , s_size) do{    \
    printDebugInfoWithoutTime("\n"); \
    printDebugInfoWithoutTime("[In file : %s , Line %d] : Output bin code :\n" , __FILE__ , __LINE__); \
    binDebugOutput(struct_ptr , s_size); \
    printDebugInfoWithoutTime("End of output bin code!\n\n"); \
}while(0)

#define DEBUG_BIN_CODE_TIME(struct_ptr , s_size) do{    \
    printDebugInfoWithoutTime("\n"); \
    printDebugInfo("[In file : %s , Line %d] : Output bin code :\n" , __FILE__ , __LINE__); \
    binDebugOutput(struct_ptr , s_size); \
    printDebugInfo("End of output bin code!\n\n"); \
}while(0)

#endif

#define DEBUG_OUTPUT(format , ...)  do{ \
    printDebugInfoWithoutTime("[In file : %s , Line %d] : " , __FILE__ , __LINE__); \
    printDebugInfoWithoutTime(format , __VA_ARGS__); \
    printDebugInfoWithoutTime("\n"); \
}while(0)

#if(DEBUG_LEVEL > 0)

#define LOG_INFO(format , ...)
#define LOG_INFO_TIME(format , ...)

#else 

#define LOG_INFO(format , ...) do{ \
    printDebugInfoWithoutTime("[ DEBUG_INFO ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#define LOG_INFO_TIME(format , ...) do{ \
    printDebugInfo("[ DEBUG_INFO ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#endif


#if(DEBUG_LEVEL > 1)

#define LOG_WARNING(format , ...)
#define LOG_WARNING_TIME(format , ...)

#else

#define LOG_WARNING(format , ...) do{ \
    printDebugInfoWithoutTime("[ ~~DEBUG_WARNING~~ ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#define LOG_WARNING_TIME(format , ...) do{ \
    printDebugInfo("[ ~~DEBUG_WARNING~~ ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#endif

#if(DEBUG_LEVEL > 2)

#define LOG_ERROR(format , ...)
#define LOG_ERROR_TIME(format , ...)

#else

#define LOG_ERROR(format , ...) do{ \
    printDebugInfoWithoutTime("[ !!!DEBUG_ERROR!!! ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#define LOG_ERROR_TIME(format , ...) do{ \
    printDebugInfo("[ !!!DEBUG_ERROR!!! ] : "); \
    DEBUG_OUTPUT(format , __VA_ARGS__); \
}while(0)

#endif


typedef enum 
{
    OUT_SCR = 0 ,
    OUT_FILE , 
    OUT_BOTH
}OUT_TYPE;

typedef struct 
{
    OUT_TYPE type;
    FILE *fp;
}MYDEBUG;

static MYDEBUG *my_debug_ptr;
static int is_hint;

void beginDebug(char * , OUT_TYPE type);

void endDebug();

int mySprintf(char * , int , char * , ...);

void set_output_type(OUT_TYPE type);

void printDebugInfo(char * , ...);

void printDebugInfoWithoutTime(char *format , ...);

void do_print_debug_info(char *msgBuf , char *timeBuf);

void getTime(char * , int );

void binDebugOutput(void * , int );

#endif
