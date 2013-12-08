
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#define INFO_LENGTH     256
#define ONE_LOG_LENGTH  512

#define DO_PRINT_ARGS(buf , len , ret , format)    do{ \
    va_list arg_list; \
    va_start(arg_list , format); \
    ret = vsnprintf(buf , len , format , arg_list); \
    va_end(arg_list); \
}while(0)

static int log_type_inside = -1;
static int file_descriptor_inside = -1;
static int log_level_inside = INFO_MSG;
#define DEFAULT_LOG_FILE "default_log_file"

static char *msg_type[] = 
{
    (char *)"INFO" , 
    (char *)"WARNING" , 
    (char *)"ERROR" , 
    (char *)"SYSCALL" , 
    (char *)"EXIT" , 
    (char *)"UNDEFINED"
};

void start_debug(int type , int level , const char *filename)
{
    if((type >= OUT_SCR) && (type <= OUT_BOTH))
        log_type_inside = type;
    else
        log_type_inside = OUT_SCR;

    if(level > ERROR_MSG || level < INFO_MSG)
        log_level_inside = INFO_MSG;
    else 
        log_level_inside = level;

    if(NULL == filename)
        filename = (char *)DEFAULT_LOG_FILE;

    int fd = open(filename , O_RDWR | O_APPEND | O_CREAT , 0644);
    if(fd < 0)
    {
        printf("open log file %s error : %s\n" , filename , strerror(errno));
        exit(-1);
    }
    
    file_descriptor_inside = fd;
}

#define SYSCALL_INFO_LENGTH 64
void print_info(MSG_TYPE type_nr , int line , const char *file , const char *format , ...)
{
    if(-1 == log_type_inside)
    {
        printf("use START_DEBUG() init debug first !\n");
        return ;
    }

    if(log_level_inside > INFO_MSG && INFO_MSG == type_nr)
    {
        return ;
    }
    else if(log_level_inside > WARNING_MSG && WARNING_MSG > type_nr)
        return ;

    char syscall_info[SYSCALL_INFO_LENGTH];
    /*  
#if((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE)
    int str_len = strerror_r(errno , syscall_info , SYSCALL_INFO_LENGTH);
    syscall_info[str_len] = '\0';
#else
    char *tmp = strerror_r(errno , syscall_info , SYSCALL_INFO_LENGTH);
    memcpy(syscall_info , tmp , strlen(tmp) + 1);
#endif
*/
    char *tmp = strerror(errno);
    memcpy(syscall_info , tmp , strlen(tmp) + 1);


    char *type = msg_type[type_nr];
    char output[ONE_LOG_LENGTH];
    memset(output , 0 , ONE_LOG_LENGTH);

    int length = 0;
    char info[INFO_LENGTH];
    memset(info , 0 , INFO_LENGTH);
    DO_PRINT_ARGS(info , INFO_LENGTH , length , format);

    if((SYSCALL_MSG != type_nr) && (SYSCALL_EXIT_MSG != type_nr))
        length = snprintf(output , ONE_LOG_LENGTH , "[%s]:%s [in file %s line %d]\n" , 
                type , info , file , line);
    else
    {
        length = snprintf(output , ONE_LOG_LENGTH , "[%s]:%s : %s [in file %s line %d]\n" 
                , type , info , strerror(errno) , file , line);
    }

    if(log_type_inside & OUT_FILE)
        write(file_descriptor_inside , output , length);
    if(log_type_inside & OUT_SCR)
        write(STDOUT_FILENO , output , length);
    
    if((EXIT_MSG == type_nr) || (SYSCALL_EXIT_MSG == type_nr))
        exit(0);
}

void end_debug()
{
    if(file_descriptor_inside > 0)
    {
        close(file_descriptor_inside);
        file_descriptor_inside = -1;
    }

    log_type_inside = -1;
}


#define MY_MAX_LINE 2048
#define COUT_ONE_LINE 16
#define ADDR_LENGTH 16
#define BIN_CODE_LENGTH     (COUT_ONE_LINE * 2 + COUT_ONE_LINE / 2)
#define IS_PRINTFUL(ch)  ((ch) > ' ' && (ch) < '~')

/* 将原始的字符串和相对于的地址以ASCII形式保存在一段缓存中。
 * 参数 outputBuffer 是保存结果的缓存。
 * 参数 maxSize 是这段缓存的大小。
 * 参数 resBuffer 是原始的字符串。
 * 参数 length 是原始字符串的长度。
 * P.S.这个函数式内部函数，所以不需要安全性检查
 */
int doOutputBin(char *outputBuffer , int maxSize , const char *resBuffer , int length)
{
    char addrBuffer[ADDR_LENGTH];             //保存每次打印的地址信息
    int  addrCount = 0;              //地址字符串的长度
    char binBuffer[BIN_CODE_LENGTH];             //打印这些字符的ASCII码的信息
    int  binCount = 0;               //二进制码信息长度
    char asciiBuffer[COUT_ONE_LINE + 1];           //保存原始字符的信息
    int  asciiCount = 0;             //原始字符的长度

    addrCount = snprintf(addrBuffer , 32 , "[ %p ]" , resBuffer);         //首先得到这段原始字符串的首地址。

    int i = 0;
    for(i = 0 ; i < length ; ++i)               
    {
        char tempChar = *(resBuffer + i);
        binCount += snprintf(binBuffer + binCount , 128 - binCount , "%02X" , (unsigned char )tempChar);         //将每一个字节的转换成ASCII形式。
        if(0 == (i + 1) % 2)
            binCount += snprintf(binBuffer + binCount , 128 - binCount , " ");                                   //两个字符之间使用一个空格

        if(!IS_PRINTFUL(tempChar))         //不可打印字符使用这个字符表示
            tempChar = '@';

        asciiCount += snprintf(asciiBuffer + asciiCount , 128 - asciiCount , "%c" , tempChar);                   //保存原始字符，因为参数不是以'\0'结束的。            
    }
    
    while(binCount < BIN_CODE_LENGTH)                        //在ASCII字符串之后补全空格
        binCount += snprintf(binBuffer + binCount , 128 - binCount , " ");

    while(addrCount < ADDR_LENGTH)                           //将地址字符串补全为指定的字节数。
        addrCount += snprintf(addrBuffer + addrCount , 32 - addrCount , "-");

    int allCount = snprintf(outputBuffer , maxSize , "%s----%s   %s\n" , addrBuffer , binBuffer , asciiBuffer);

    outputBuffer[allCount] = '\0';

    return i;
}

/* 将传入的一段字符串以ASCII编码的形式输出。
 * 参数 debug 是指定的句柄
 * 参数 structBuffer 是这段内存的首地址。
 * 参数 nSize 是这段内存的需要打印的长度。
 */
void print_binary_code(const void *structBuffer , int nSize)
{
    if(0 == log_type_inside)
    {
        printf("use START_DEBUG() init debug first !\n");
        return ;
    }
    if(log_level_inside != INFO_MSG)
        return ;

    char outputBuf[MY_MAX_LINE];
    int beginAddr = 0;
    int leftCount = nSize;
    const char *sBuffer = (const char *)structBuffer;

    if(nSize <= 0 || NULL == structBuffer)
    {
        LOG_WARNING("Print binary need legal input string and length !");
        return ;
    }

    while(leftCount > 0)
    {
        int outputCount = 0;
        if(leftCount > COUT_ONE_LINE)
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , COUT_ONE_LINE);
        else
            outputCount = doOutputBin(outputBuf , MY_MAX_LINE , sBuffer + beginAddr , leftCount);

        if(log_type_inside & OUT_FILE)
            write(file_descriptor_inside , outputBuf , strlen(outputBuf));
        if(log_type_inside & OUT_SCR)
            write(STDOUT_FILENO , outputBuf , strlen(outputBuf));


        beginAddr += outputCount;
        leftCount -= outputCount;
    }
}
