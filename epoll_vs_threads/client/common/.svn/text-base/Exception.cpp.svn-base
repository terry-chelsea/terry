/*
 * =====================================================================================
 *
 *     FileName:  Exception.cpp
 *  Description:  implementation of Exception class
 *      Version:  1.0
 *      Created:  Tue Apr 13 22:31:49 2010

 *     Revision:  none
 *     Compiler:  g++
 *
 *       Author:  ptz, ptz@uestc.edu.cn
 *      Company:  NDSL UESTC
 * =====================================================================================
 */

#include "Exception.h"
#include <iostream>
#include "Tool.h"
#include "debug.h"

using namespace std;

Exception::Exception(const char* str, const char* file, const char* func, int line):
        m_ExStr(str),
        m_File(file),
        m_Func(func),
        m_Line(line)
{
}

Exception::Exception(const string &str, const char* file, const char* func, int line):
        m_ExStr(str),
        m_File(file),
        m_Func(func),
        m_Line(line)
{
}

Exception::~Exception()
{
}

void Exception::print()
{    
    LOG_ERROR("%s [in %s on line %d]" , m_ExStr.c_str() , m_File.c_str() , m_Line);
}
