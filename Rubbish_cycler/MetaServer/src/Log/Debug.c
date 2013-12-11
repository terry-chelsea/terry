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

/* ����ʽ���ַ������뵽һ��������ȥ��...
 * ���� resBuf ָ�����������
 * ���� nSize ָ������������Ĵ�С
 * ���������Ǹ�ʽ���ַ�����Ϣ
 * ��������ֵΪ��Ч���ַ����Ĵ�С
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

    while((debug = (MYDEBUG *)malloc(sizeof(MYDEBUG))) == NULL);              //��ִ֤�гɹ�

    memset(debug , 0 , sizeof(MYDEBUG));

    int length = 0;
    if(NULL == appName)
        length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "Unknow_App_Debug_File");
    else 
        length = mySprintf(myFileName , MY_FILE_NAME_LENGTH , "%s_Debug_File" , appName);
    myFileName[length] = '\0';

    //delete the file first...
    remove(myFileName);

    while((debug->fp = fopen(myFileName , "a+")) == NULL);                    //��֤����ִ�гɹ�
    
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


/* ���ƶ������ݸ�ʽ�����
 * �����Ǹ�ʽ����ʽ���ַ������ɱ����
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



/* ��ԭʼ���ַ���������ڵĵ�ַ��ASCII��ʽ������һ�λ����С�
 * ���� outputBuffer �Ǳ������Ļ��档
 * ���� maxSize ����λ���Ĵ�С��
 * ���� resBuffer ��ԭʼ���ַ�����
 * ���� length ��ԭʼ�ַ����ĳ��ȡ�
 * P.S.�������ʽ�ڲ����������Բ���Ҫ��ȫ�Լ��
 */
static int doOutputBin(char *outputBuffer , int maxSize , char *resBuffer , int length)
{
    int putCount = 0;
    char addrBuffer[ADDR_LENGTH];             //����ÿ�δ�ӡ�ĵ�ַ��Ϣ
    int  addrCount = 0;              //��ַ�ַ����ĳ���
    char binBuffer[BIN_CODE_LENGTH];             //��ӡ��Щ�ַ���ASCII�����Ϣ
    int  binCount = 0;               //����������Ϣ����
    char asciiBuffer[COUT_ONE_LINE + 1];           //����ԭʼ�ַ�����Ϣ
    int  asciiCount = 0;             //ԭʼ�ַ��ĳ���

    addrCount = mySprintf(addrBuffer , 32 , "[ %0X ]" , resBuffer);         //���ȵõ����ԭʼ�ַ������׵�ַ��

    int i = 0;
    for(i = 0 ; i < length ; ++i)               
    {
        char tempChar = *(resBuffer + i);
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , "%02X" , (unsigned char )tempChar);         //��ÿһ���ֽڵ�ת����ASCII��ʽ��
        if(0 == (i + 1) % 2)
            binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");                                   //�����ַ�֮��ʹ��һ���ո�

        if(!IS_PRINTFUL(tempChar))         //���ɴ�ӡ�ַ�ʹ������ַ���ʾ
            tempChar = '@';

        asciiCount += mySprintf(asciiBuffer + asciiCount , 128 - asciiCount , "%c" , tempChar);                   //����ԭʼ�ַ�����Ϊ����������'\0'�����ġ�            
    }
    
    while(binCount < BIN_CODE_LENGTH)                        //��ASCII�ַ���֮��ȫ�ո�
        binCount += mySprintf(binBuffer + binCount , 128 - binCount , " ");

    while(addrCount < ADDR_LENGTH)                           //����ַ�ַ�����ȫΪָ�����ֽ�����
        addrCount += mySprintf(addrBuffer + addrCount , 32 - addrCount , "-");

    int allCount = mySprintf(outputBuffer , maxSize , "%s----%s   %s" , addrBuffer , binBuffer , asciiBuffer);

    outputBuffer[allCount + 1] = '\0';
    outputBuffer[allCount] = '\n';

    return i;
}

/* �������һ���ַ�����ASCII�������ʽ�����
 * ���� debug ��ָ���ľ��
 * ���� structBuffer ������ڴ���׵�ַ��
 * ���� nSize ������ڴ����Ҫ��ӡ�ĳ��ȡ�
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
