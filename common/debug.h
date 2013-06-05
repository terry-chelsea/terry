/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/13 10:58:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_DEBUG_H_
#define _H_DEBUG_H_

#include <stdio.h>

#define PRINT(format , ...)  do{\
    printf(format , ##__VA_ARGS__); \
	printf("\n"); \
}while(0)

#define LOG_INFO(format , ...)    \
	PRINT(format , ##__VA_ARGS__)

#define LOG_WARNING(format , ...) \
	PRINT(format , ##__VA_ARGS__)

#define LOG_ERROR(format , ...) \
	PRINT(format , ##__VA_ARGS__)

#define LOG_SYSCALL(format , ...) \
	PRINT(format , ##__VA_ARGS__)

#define LOG_SYSCALL_EXIT(format , ...) \
	PRINT(format , ##__VA_ARGS__)

#endif
