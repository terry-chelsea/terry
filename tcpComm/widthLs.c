/*
 * =====================================================================================
 *
 *       Filename:  myls.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/13/12 16:20:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "terryTcpLib.h"

int doReg(char *path)
{
    int fd = open(path , O_RDONLY);
    if(fd < 0)
    {
        fprintf(stderr , "********WARNING : open file %s error : %s *********\n" , path , strerror(errno));
        return 0;
    }

    char ch = 0;
    if(read(fd , &ch , sizeof(ch)) < 0)
    {
        fprintf(stderr , "********WARNING : read file %s error : %s ********\n" , path , strerror(errno));
        close(fd);
        return 0;
    }

//    fprintf(stdout , "Regular file : %s\n" , path);

//    close(fd);
    return 0;
}

int doOth(char *path)
{
//   fprintf(stdout , "Other type : %s\n" , path);
    return 0;
}

extern char *program_name;

int main(int argc , char *argv[])
{
    if(argc != 2)
    {
        printf("error input\n");
        exit(-1);
    }
    INIT();

    dealWithInput(argv[1]);

    widthTraverseDir(argv[1] , doReg , doOth);

    return 0;
}
