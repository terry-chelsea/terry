/*
 * =====================================================================================
 *
 *       Filename:  ConsoleAppender.cpp
 *
 *    Description:  日志输出器，输出到控制台（输出到标准错误输出）
 *
 *        Version:  1.0
 *        Created:  04/24/2011 02:40:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "ConsoleAppender.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <string>

#include "LogLevel.h"

#define LOG_BUFFER_SIZE 6000

void ConsoleAppender::log(
        std::string loggerName, 
        LogLevel level, 
        const char *fmt, 
        va_list ap,
        const char *file,
        int line, 
        const char *function) {
    char buffer[LOG_BUFFER_SIZE];
    
    // time, levle and logger name
    snprintf(buffer, LOG_BUFFER_SIZE, "%s %s %s %s - ", 
            getTimeNow().c_str(), 
            getLogLevelString(level).c_str(),
            getPIDAndTID().c_str(),
            loggerName.c_str());
    
    // log data
    int len = strlen(buffer);
    vsnprintf(buffer + len, LOG_BUFFER_SIZE - len, fmt, ap);

    len = strlen(buffer);
    // 添加换行
    buffer[len++] = '\n';
    buffer[len] = '\0';
    if (level == LOG_DEBUG && file != NULL && line != 0 && function != NULL) {
        // 去掉换行
        while (buffer[len - 2] == '\n') {
            --len;
        }
        buffer[len] = '\0';

        snprintf(buffer + len, LOG_BUFFER_SIZE - len,
                " (IN FILE: %s, LINE: %d, FUNCTION: %s)\n",
                file, line, function);
    }

    len = strlen(buffer);
    // 去掉多余的空行
    while (buffer[len - 2] == '\n') {
        --len;
    }
    buffer[len] = '\0';

    // 输出到stderr
    fprintf(stderr, buffer);
}

