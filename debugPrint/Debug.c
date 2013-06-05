/*
 * =====================================================================================
 *
 *       Filename:  Debug.cc
 *
 *    Description:  functions of debug 
 *
 *        Version:  1.0
 *        Created:  03/28/12 23:00:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "Debug.h"

#define DO_ARG(resBuf , size , count , format) do{\
        va_list pArgList; \
        va_start(pArgList , format); \
        count += vsnprintf(resBuf + count , size - count , format , pArgList);\
        va_end(pArgList);\
}while(0)

#ifdef DEBUG
#define CON_PRINT printf
#else
#define CON_PRINT /\
    /printf
#endif

#define IS_PRINTFUL(ch)  ((ch) > 31 && (ch) < 127)

//put all format strings to a buf , and return the real characters of the buf...
int mySprintf(char *resBuf , int nSize , char *format , ...)
{
    int nCount = -1;
    if(nSize < 0 || NULL == resBuf)
        return -1;
    {
        nCount = 0;
        DO_ARG(resBuf , nSize , nCount , format);
    }

    return nCount;
}


MYDEBUG *beginDebug(char *fileName , char *appName)
{
    MYDEBUG *debug = NULL;
    char myFileName[MY_FILE_NAME_LENGTH];

    if(NULL == appName)
        goto CREATE_MY_DEBUG_RETURN;

    debug = (MYDEBUG *)malloc(sizeof(MYDEBUG));
    if(NULL == debug)
        goto CREATE_MY_DEBUG_RETURN;

    memset(debug , 0 , sizeof(MYDEBUG));
    {
        int length = 0;
        if(NULL == fileName)
            length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "%s" , appName);
        else 
            length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "%s_%s" , fileName , appName);
        myFileName[length] = '\0';
    }

    //delete the file first...
    remove(myFileName);

    debug->fp = fopen(myFileName , "a+");
    if(NULL ==  debug->fp)
        goto DEL_STRUCT_AND_RETURN;
    
    debug->isPrintfTTY = 1;
    printf("start Debug:\n\n");

    goto CREATE_MY_DEBUG_RETURN;

DEL_STRUCT_AND_RETURN:
    free(debug);
    debug = NULL;

CREATE_MY_DEBUG_RETURN :
    return debug;
}

void endDebug(MYDEBUG **debug)
{
    if(NULL == debug || NULL == *debug)
        return ;

    fclose((*debug)->fp);
    free(*debug);

    *debug = NULL;

    printf("\nend Debug...\n");
}

void getTime(char *timeBuf , int nSize)
{
    if(NULL == timeBuf || nSize < 0)
        return ;
    
    time_t timeVal;
    struct tm *pTime = NULL;
    time(&timeVal);
    pTime = localtime(&timeVal);

    int nLength = mySprintf(timeBuf , nSize , "%s" , asctime(pTime));

    timeBuf[nLength - 1] = '\0';

    return ;
}

void closeTTY(MYDEBUG *debug)
{
    if(NULL == debug)
        return ;

    debug->isPrintfTTY = 0;
}


int printDebugInfo(MYDEBUG *debug , char *format , ...)
{
    int nCount = -1;
    char msgBuf[MY_MAX_LINE];
    char timeBuf[MY_MAX_LINE];
    int nSize = MY_MAX_LINE;

    if(NULL == debug || NULL == debug->fp || NULL == format)
        return  -1;

    {
        nCount = 0;
        DO_ARG(msgBuf , nSize , nCount , format);
    }

    msgBuf[nCount] = '\0';
    getTime(timeBuf , MY_MAX_LINE);
    
    nCount = fprintf(debug->fp , "[%s] : %s\n" , timeBuf , msgBuf);
    if(debug->isPrintfTTY)
    {
        printf("[%s] : %s\n" , timeBuf , msgBuf);
    }

    return nCount;
}

//put resBuffer address , bin code and ascii code to outputBuffer...
static int doOutputBin(char *outputBuffer , int maxSize , char *resBuffer , int length)
{
    int putCount = 0;
    char addrBuffer[32];
    int  addrCount = 0;
    char binBuffer[128];
    int  binCount = 0;
    char asciiBuffer[128];
    int  asciiCount = 0;

    addrCount = mySprintf(addrBuffer , 32 , "%0X" , resBuffer);

    int i = 0;
    for(i = 0 ; i < length ; ++i)
    {
        char tempChar = *(resBuffer + i);
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , "%02X" , (unsigned char )tempChar);
        if(0 == (i + 1) % 2)
            binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");

        if(!IS_PRINTFUL(tempChar))
            tempChar = '@';

        asciiCount += mySprintf(asciiBuffer + asciiCount , 128 - asciiCount , "%c" , tempChar);
            
    }
    
    while(binCount < BIN_CODE_LENGTH)
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");

    while(addrCount < ADDR_LENGTH)
        addrCount += mySprintf(addrBuffer + addrCount , 32 - addrCount , " ");

    int allCount = mySprintf(outputBuffer , maxSize , "%s----%s   %s" , addrBuffer , binBuffer , asciiBuffer);

    outputBuffer[allCount + 1] = '\0';
    outputBuffer[allCount] = '\n';

    return i;
}

void binDebugOutput(MYDEBUG *debug , void *structBuffer , int nSize)
{
    char outputBuf[MY_MAX_LINE];
    int i = 0;
    int beginAddr = 0;
    int leftCount = nSize;
    char *sBuffer = (char *)structBuffer;

    if(nSize <= 0 || NULL == structBuffer)
        return ;

    while(leftCount > 0)
    {
        int outputCount = 0;
        if(leftCount > COUT_ONE_LINE)
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , COUT_ONE_LINE);
        else
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , leftCount);

        printDebugInfo(debug , "%s" , outputBuf);

        beginAddr += outputCount;
        leftCount -= outputCount;
    }

    return ;
}
