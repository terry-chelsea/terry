/*
 * =====================================================================================
 *
 *       Filename:  sprintf.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/12 22:28:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<string.h>

void sprintfTest()
{
    int i = 0;

    char buf[128];
    for(i = 0 ; i < 128 ; ++i)
        buf[i] = '*';
    buf[127] = '\0';

    char rBuf[256];
    for(i = 0 ; i < 256 ; ++i)
        rBuf[i] = '+';
    rBuf[255] = '\0';

    int rtn = snprintf(buf , 100 , rBuf);

    rtn = strlen(buf);

    printf("%s and rtn = %d\n" , buf , rtn);

}

int main()
{
    sprintfTest();

    return 0;
}
