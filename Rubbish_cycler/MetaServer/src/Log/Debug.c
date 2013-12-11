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
        count = vsnprintf(resBuf , size , format , pArgList);\
        va_end(pArgList);\
}while(0)

#ifdef DEBUG
#define CON_PRINT printf
#else
#define CON_PRINT /\
    /printf
#endif

#define IS_PRINTFUL(ch)  ((ch) > 31 && (ch) < 127)

/* 将格式化字符串输入到一个缓冲区去中...
 * 参数 resBuf 指定输出缓冲区
 * 参数 nSize 指定输出缓冲区的大小
 * 其他参数是格式化字符串信息
 * 函数返回值为有效地字符串的大小
 * */
int mySprintf(char *resBuf , int nSize , char *format , ...)
{
    int nCount = -1;
    if(nSize < 0 || NULL == resBuf)
        return -1;

    nCount = 0;
    DO_ARG(resBuf , nSize , nCount , format);

    return nCount;
}


void beginDebug(char *appName , OUT_TYPE type)
{
    MYDEBUG *debug = NULL;
    char myFileName[MY_FILE_NAME_LENGTH];

    while((debug = (MYDEBUG *)malloc(sizeof(MYDEBUG))) == NULL);              //保证执行成功

    memset(debug , 0 , sizeof(MYDEBUG));

    int length = 0;
    if(NULL == appName)
        length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "Unknow_App_Debug_File");
    else 
        length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "%s_Debug_File" , appName);
    myFileName[length] = '\0';

    //delete the file first...
    remove(myFileName);

    while((debug->fp = fopen(myFileName , "a+")) == NULL);                    //保证总是执行成功
    
    debug->type = type;
    my_debug_ptr = debug;

    char timeBuf[MY_MAX_LINE];
    getTime(timeBuf , MY_MAX_LINE);
    fprintf(my_debug_ptr->fp , "-------------------[%s] : START Debug...-------------------\n" , timeBuf);

    is_hint = 1;
}

void endDebug()
{
    char timeBuf[MY_MAX_LINE];
    getTime(timeBuf , MY_MAX_LINE);
    fprintf(my_debug_ptr->fp , "-------------------[%s] : FINISH_Debug...-------------------\n" , timeBuf);

    if(NULL == my_debug_ptr)
        return ;

    fclose(my_debug_ptr->fp);
    free(my_debug_ptr);

    my_debug_ptr = NULL;
    is_hint = 0;
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

void set_output_type(OUT_TYPE type)
{
    if(NULL == my_debug_ptr)
        return ;

    my_debug_ptr->type = type;
}


/* 将制定的内容格式化输出
 * 参数是格式化形式的字符串，可变参数
 */
void printDebugInfo(char *format , ...)
{
    int nCount = -1;
    char msgBuf[MY_MAX_LINE];
    char timeBuf[MY_MAX_LINE];
    int nSize = MY_MAX_LINE;

    if(NULL == format)
    {
        fprintf(stderr , "Hint : error input in printDebugInfo()!\n");
        return ;
    }

    nCount = 0;
    DO_ARG(msgBuf , nSize , nCount , format);

    msgBuf[nCount] = '\0';
    getTime(timeBuf , MY_MAX_LINE);
    
    do_print_debug_info(msgBuf , timeBuf);
}

#ifdef DEBUG

void do_print_debug_info(char *msgBuf , char *timeBuf)
{
    if(NULL == my_debug_ptr)
    {
        if(0 == is_hint)
        {
            fprintf(stderr , "Hint : create DEBUG first !\n");
            is_hint = 1;
            return ;
        }
        else
        {
            return ;
        }
    }

    if(NULL == msgBuf)
        return ;
    if(NULL == timeBuf)
    {
        if(OUT_BOTH == my_debug_ptr->type)
        {
            fprintf(my_debug_ptr->fp , "%s" , msgBuf);
            fprintf(stdout , "%s" , msgBuf);
        }
        else if(OUT_FILE == my_debug_ptr->type)
        {
            fprintf(my_debug_ptr->fp , "%s" , msgBuf);
        }
        else
        {
            fprintf(stdout , "%s" , msgBuf);
        }
        return ;
    }

    if(OUT_BOTH == my_debug_ptr->type)
    {
        fprintf(my_debug_ptr->fp , "[%s] : %s" , timeBuf , msgBuf);
        fprintf(stdout , "[%s] : %s" , timeBuf , msgBuf);
    }
    else if(OUT_FILE == my_debug_ptr->type)
    {
        fprintf(my_debug_ptr->fp , "[%s] : %s" , timeBuf , msgBuf);
    }
    else
    {
        fprintf(stdout , "[%s] : %s" , timeBuf , msgBuf);
    }

}

#else

void do_print_debug_info(char *msgBuf , char *timeBuf)
{

}

#endif


void printDebugInfoWithoutTime(char *format , ...)
{
    char msgBuf[MY_MAX_LINE];
    int nSize = MY_MAX_LINE;

    if(NULL == format)
    {
        fprintf(stderr , "Hint : error input in printDebugInfo()!\n");
        return ;
    }

    int nCount = 0;
    DO_ARG(msgBuf , nSize , nCount , format);

    msgBuf[nCount] = '\0';
    
    do_print_debug_info(msgBuf , NULL);
}



/* 将原始的字符串和相对于的地址以ASCII形式保存在一段缓存中。
 * 参数 outputBuffer 是保存结果的缓存。
 * 参数 maxSize 是这段缓存的大小。
 * 参数 resBuffer 是原始的字符串。
 * 参数 length 是原始字符串的长度。
 * P.S.这个函数式内部函数，所以不需要安全性检查
 */
static int doOutputBin(char *outputBuffer , int maxSize , char *resBuffer , int length)
{
    int putCount = 0;
    char addrBuffer[ADDR_LENGTH];             //保存每次打印的地址信息
    int  addrCount = 0;              //地址字符串的长度
    char binBuffer[BIN_CODE_LENGTH];             //打印这些字符的ASCII码的信息
    int  binCount = 0;               //二进制码信息长度
    char asciiBuffer[COUT_ONE_LINE + 1];           //保存原始字符的信息
    int  asciiCount = 0;             //原始字符的长度

    addrCount = mySprintf(addrBuffer , 32 , "[ %0X ]" , resBuffer);         //首先得到这段原始字符串的首地址。

    int i = 0;
    for(i = 0 ; i < length ; ++i)               
    {
        char tempChar = *(resBuffer + i);
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , "%02X" , (unsigned char )tempChar);         //将每一个字节的转换成ASCII形式。
        if(0 == (i + 1) % 2)
            binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");                                   //两个字符之间使用一个空格

        if(!IS_PRINTFUL(tempChar))         //不可打印字符使用这个字符表示
            tempChar = '@';

        asciiCount += mySprintf(asciiBuffer + asciiCount , 128 - asciiCount , "%c" , tempChar);                   //保存原始字符，因为参数不是以'\0'结束的。            
    }
    
    while(binCount < BIN_CODE_LENGTH)                        //在ASCII字符串之后补全空格
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");

    while(addrCount < ADDR_LENGTH)                           //将地址字符串补全为指定的字节数。
        addrCount += mySprintf(addrBuffer + addrCount , 32 - addrCount , "-");

    int allCount = mySprintf(outputBuffer , maxSize , "%s----%s   %s" , addrBuffer , binBuffer , asciiBuffer);

    outputBuffer[allCount + 1] = '\0';
    outputBuffer[allCount] = '\n';

    return i;
}

/* 将传入的一段字符串以ASCII编码的形式输出。
 * 参数 debug 是指定的句柄
 * 参数 structBuffer 是这段内存的首地址。
 * 参数 nSize 是这段内存的需要打印的长度。
 */
void binDebugOutput(void *structBuffer , int nSize)
{
    char outputBuf[MY_MAX_LINE];
    int i = 0;
    int beginAddr = 0;
    int leftCount = nSize;
    char *sBuffer = (char *)structBuffer;

    if(nSize <= 0 || NULL == structBuffer)
    {
        fprintf(stderr , "Hint : input error of binDebugOutput()!\n");
        return ;
    }

    while(leftCount > 0)
    {
        int outputCount = 0;
        if(leftCount > COUT_ONE_LINE)
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , COUT_ONE_LINE);
        else
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , leftCount);

        printDebugInfoWithoutTime("%s" , outputBuf);

        beginAddr += outputCount;
        leftCount -= outputCount;
    }
}
