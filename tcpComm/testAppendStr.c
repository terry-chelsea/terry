/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/13/12 15:43:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<assert.h>
#include<string.h>

#define DELIMITER_CHAR '/'
#define INIT   "//////"

void appendNewPath(char *destPath , char *newPath)
{
    int newSize = strlen(newPath);
    int destSize = strlen(destPath);

    if('\0' == *newPath)
    {
        *destPath = DELIMITER_CHAR;
        memcpy(destPath + 1 , newPath , newSize);
        return ;
    }
    destPath[destSize] = DELIMITER_CHAR;
    memcpy(destPath + destSize + 1 , newPath , newSize);
    destPath[destSize + 1 + newSize] = '\0';
}

void clearNewPath(char *path)
{
    int pathSize = strlen(path);

    char *endPtr = path + pathSize - 1;
    while((*endPtr != DELIMITER_CHAR) && (endPtr >= path))
        --endPtr;
    if(endPtr != path)
        *endPtr = '\0';
    else 
        *endPtr = DELIMITER_CHAR;
}

void dealWithInput(char *path)
{
    int pathSize = strlen(path);
    
    char *endPtr = path + pathSize - 1;
    while((*endPtr == DELIMITER_CHAR) && (endPtr > path))
        --endPtr;

    if(endPtr == path)
        *path = '\0';
    else
        *(endPtr + 1) = '\0';
}

int main(int argc , char *argv[])
{
    assert(argc >= 3);

    char result[1000];
    memcpy(result , INIT , sizeof(INIT));

    dealWithInput(result);

    int i = 1;
    while(i < argc)
    {
        appendNewPath(result , argv[i]);
        ++ i;
    }

    clearNewPath(result);

    printf("append string is %s\n" , result);

    clearNewPath(result);

    printf("append string is %s\n" , result);

    clearNewPath(result);

    printf("append string is %s\n" , result);

    return 0;
}
