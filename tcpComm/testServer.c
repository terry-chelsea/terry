/*
 * =====================================================================================
 *
 *       Filename:  testServer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/13/12 21:17:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "terryTcpLib.h"
#include<signal.h>

#define PORT "1926"

void sig_pipe(int signo)
{
    printf("get SIGPIPE\n");
    return ;
}

int main(int argc , char *argv[])
{
    INIT();

    signal(SIGPIPE , sig_pipe);

    int serv = tcpServer(NULL , PORT);

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    memset(&client , 0 , len);

    int connfd = accept(serv , (struct sockaddr *)&client , &len);

    int rlen = 0;
    char input[128];
    while((rlen = read(connfd , input , 128)) > 0)
    {
        printf("read length : %d\n" , rlen);
        printf("client read : %s\n" , input);

        sleep(5);

        write(connfd , input , rlen);
        sleep(5);
    }

    return 0;
}
