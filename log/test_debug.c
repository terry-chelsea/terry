/*
 * =====================================================================================
 *
 *       Filename:  test_debug.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/12 23:19:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "Debug.h"

int main(int argc , char *argv[])
{
    char buf[128] = "this is just a line for test the out put thing in bin and ascii code...";

//    int rtn = mySprintf(buf , 128 , "hello world from %d , and line is %s" , 16 , "feng yu");

//    printf("%s\n" , buf);

//    printf("rtn is %d\n" , rtn);
    
    START_DEBUG(argv[0] , 2);

    DEBUG_BIN_CODE(buf , strlen(buf));

    char someting[100];
    DEBUG_BIN_CODE_TIME(someting , sizeof(someting));

    LOG_INFO("Hello %d world %d" , 16 , 116);

    LOG_WARNING("Hello %d world %d" , 16 , 116);

    LOG_ERROR("Hello %d world %d" , 16 , 116);

    LOG_ERROR_TIME("Hello %d world %d" , 16 , 116);

    FINISH_DEBUG();

    return 0;
}
