/*
 * =====================================================================================
 *
 *       Filename:  testClient.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/13/12 21:29:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "terryTcpLib.h"


#define IP "192.168.9.64"
#define PORT "1926"

int main(int argc , char *argv[])
{
    int client = tcpClient(IP , PORT);


    char input[128] = "hello world from feng xiao yu...";

//    while(fgets(input , 128 , stdin) != NULL)
    while(1)    
    {
        int wlen = 0;
        wlen = write(client , input , strlen(input) + 1);
        if(wlen < 0)
            error(EXIT_ERROR , PRINT_ERRNO , "write error");

        memset(input , 0 , 128);
        int rlen = read(client , input , sizeof(input));
        printf("read length  is %d\n" , rlen);

        printf("read : %s\n" , input);
    }

    return 0;

}
