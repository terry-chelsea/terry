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

int main()
{
    char buf[128] = "this is just a line for test the out put thing in bin and ascii code...";

//    int rtn = mySprintf(buf , 128 , "hello world from %d , and line is %s" , 16 , "feng yu");

//    printf("%s\n" , buf);

//    printf("rtn is %d\n" , rtn);
    MYDEBUG *debug = beginDebug("for" , "debug");
    
    printDebugInfo(debug , "this is a bug in file %s and line %d\n" , "test_debug" , 16);    

    binDebugOutput(debug , buf , strlen(buf));

    binDebugOutput(debug , debug , sizeof(*debug));


    char someting[100];
    binDebugOutput(debug , someting , sizeof(someting));

    endDebug(&debug);

    return 0;
}
