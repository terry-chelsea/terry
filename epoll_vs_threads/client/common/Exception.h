/*
 * =====================================================================================
 *
 *     FileName:  Exception.h
 *  Description:
 *      Version:  1.0
 *      Created:  Tue Apr 13 22:31:49 2010

 *     Revision:  none
 *     Compiler:  g++
 *
 *       Author:  ptz, ptz@uestc.edu.cn
 *      Company:  NDSL UESTC
 * =====================================================================================
 */

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
#include <string>
class Exception
{
public:
    Exception(const char* str, const char* file, const char* func, int line);
    Exception(const std::string &str, const char* file, const char* func, int line);
    ~Exception();
    void print();

private:
    std::string m_ExStr;
    std::string m_File;
    std::string m_Func;
    int m_Line;
};

#define  THROW_EX(what) throw  Exception(what, __FILE__,__FUNCTION__,__LINE__);

#endif
