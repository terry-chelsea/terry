/*
 * =====================================================================================
 *
 *       Filename:  atom.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/04/12 09:43:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "atom.h"

extern BUSKET initBusket;

unsigned long getHash(const char *str , int len)
{
    unsigned long hash = 0;
    int i = 0 ;
    for(i = 0 ; i < len ; ++ i)
    {
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & 0x7FFFFFFF);
}

unsigned long getHash2(const char *str , int len)
{
    unsigned long hash = 0;
    int i = 0 ;
    for(i = 0 ; i < len ; ++ i)
        hash += str[i] * 1713 + 2013;
    hash &= 0x7F1FFFA7;
    return (hash & 0x7FFFFFFF);
}

void atomInit(int hint)
{
    assert(hint > 0 && hint <= MAX_BUSKET);
    if(hint < MIN_BUSKET)
        hint = DEFAULT_BUSKET;

    int allocSize = sizeof(ATOM *) * hint;
    ATOM **buskets = (ATOM **)malloc(allocSize);
    memset(buskets , 0 , allocSize);

    initBusket.buskets = buskets;
    initBusket.func = getHash;
    initBusket.busketNumbers = hint;
}

const char *atomNew(const char *str , int len)
{
    assert(str != NULL);
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);
    assert(len >= 0);

    ATOM **buskets = initBusket.buskets;

    unsigned long hash = -1;
    ATOM *atomPtr = NULL;
    int index = -1;
    
    hash = (initBusket.func)(str , len);
    index = hash % initBusket.busketNumbers;
    
    atomPtr = buskets[index];
    while(atomPtr != NULL)
    {
        if(atomPtr->len == len)
        {
            //maybe it can increase speed of find...
            if(atomPtr->hashValue != hash)
            {
                atomPtr = atomPtr->next;
                continue;
            }
  
            int i = 0;
            for(i = 0 ; i < len ; ++ i)
            {
                if(atomPtr->str[i] != str[i])
                    break;
            }
            if(i == len)
                return atomPtr->str;
        }
        atomPtr = atomPtr->next;
    }
    
    atomPtr = (ATOM *)malloc(sizeof(*atomPtr) + len + 1);      //last one for null...
    memset(atomPtr , 0 , sizeof(*atomPtr));

    atomPtr->str = (char *)(atomPtr + 1);
    atomPtr->len = len;
    atomPtr->hashValue = hash;
    if(len > 0)
        memcpy(atomPtr->str , str , len);
    atomPtr->str[len] = '\0';

    atomPtr->next = buskets[index];
    buskets[index] = atomPtr;

    return atomPtr->str;
}

const char *atomString(const char *str)
{
    return atomNew(str , strlen(str));
}

const char *atomInt(const long n)
{ 
    char buf[MAX_NUMBER_SIZE];
    memset(buf , 0 , sizeof(buf));

    char *beginPtr = buf + sizeof(buf) - 1;
    long realValue = n;
    int flag = 0;

    if(n > MAX_NUMBER)
        realValue = 0;

    if(realValue < 0)
    {
        flag = 1;
        realValue = -1 * realValue;
    }
    
    if(0 == realValue)
    {
        *beginPtr = '0';
        return atomNew(beginPtr , 1);
    }
    while(realValue != 0)
    {
        *beginPtr = (realValue % 10) + '0';
        beginPtr -- ;
        realValue /= 10;
    }
    beginPtr ++;
    if(1 == flag)
        *(--beginPtr) = '-';

    return atomNew(beginPtr , (buf + sizeof(buf)) - beginPtr);
}

ATOM *atomFind(const char *str , ATOM **prev)        //store prev of find item , increase speed of free...
{
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);
    assert(str != NULL);

    ATOM **buskets = initBusket.buskets;
    ATOM *atomPtr = NULL;
    ATOM *temp = NULL;

    //for one choose , cost too much time in search...

    /*  
    for(int i = 0 ; i < initBusket.busketNumbers ; ++ i)
    {
        for(atomPtr = buskets[i] ; atomPtr != NULL ; atomPtr = atomPtr->next)
        {
            if(atomPtr->str == str)
                return atomPtr;
        }
    }
*/    

    int index = (initBusket.func)(str , strlen(str)) % initBusket.busketNumbers;
    atomPtr = buskets[index];
    if(atomPtr->str == str)
    {
        temp = NULL;
        goto out;
    }
    while(atomPtr->next != NULL)
    {
        if(atomPtr->next->str == str)
        {
            temp = atomPtr;
            atomPtr = atomPtr->next;
            goto out;
        }
        atomPtr = atomPtr->next;
    }
/*        
    for(atomPtr = buskets[index] ; atomPtr != NULL ; atomPtr = atomPtr->next)
    {
        *prev = atomPtr;
        if(atomPtr->str == str)
            goto out;
    }
*/
out:
    if(atomPtr == NULL)
        return NULL;
    if(prev != NULL)
        *prev = temp;
    return atomPtr;
}

int atomLength(const char *str)
{
    ATOM *atomPtr = atomFind(str , NULL);
    if(NULL == atomPtr)
        return -1;
    else
        return atomPtr->len;
}

int atomFree(char *str)
{
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);

    ATOM **buskets = initBusket.buskets;
    ATOM *prev = NULL;
    ATOM *atomPtr = atomFind(str , &prev);
    if(NULL == atomPtr)
        return -1;

    if(prev != NULL)
    {
        prev->next = atomPtr->next;
        free(atomPtr);
        return 0;
    }

    int index = (initBusket.func)(atomPtr->str , atomPtr->len) % initBusket.busketNumbers;
    ATOM *findPtr = buskets[index];
    if(buskets[index] == atomPtr)
    {
        buskets[index] = atomPtr->next;
        atomPtr->next = NULL;

        free(atomPtr);
        return 0;
    }
    while(findPtr->next != NULL)
    {
        if(findPtr->next == atomPtr)
        {
            findPtr->next = atomPtr->next;
            atomPtr->next = NULL;
            break;
        }
        findPtr = findPtr->next;
    }

    free(atomPtr);
    return 0;
}

void atomReset()
{
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);

    ATOM **buskets = initBusket.buskets;
    ATOM *temp = NULL;
    ATOM *atomPtr = NULL;

    int i = 0;
    for(i = 0 ; i < initBusket.busketNumbers ; ++ i)
    {
        atomPtr = buskets[i];

        while(atomPtr != NULL)
        {
            temp = atomPtr;
            atomPtr = atomPtr->next;
            free(temp);            
        }
        buskets[i] = NULL;
    }
}

void atomVload(const char *str , ...)
{
   const char *searchPtr = str;
   va_list vlist;

   va_start(vlist , str);

   while(searchPtr != NULL)
   {
       atomString(searchPtr);
       searchPtr = va_arg(vlist , char *);
   }

   va_end(vlist);
}

void atomAload(const char *strs[])
{
    char **searchPtr = (char **)strs;
    while(*searchPtr != NULL)
    {
        atomString(*searchPtr);
        searchPtr += 1;
    }
}

const char *atomAdd(const char *str , int len)
{
    assert(str != NULL);
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);
    assert(len >= 0);

    ATOM **buskets = initBusket.buskets;
    unsigned long hash = -1;
    ATOM *atomPtr = NULL;
    int index = -1;
    
    hash = (initBusket.func)(str , len);
    index = hash % initBusket.busketNumbers;
    
    atomPtr = buskets[index];
    while(atomPtr != NULL)
    {
        if(atomPtr->len == len)
        {
            int i = 0;
            for(i = 0 ; i < len ; ++ i)
            {
                if(atomPtr->str[i] != str[i])
                    break;
            }
            if(i == len)
                return atomPtr->str;
        }
        atomPtr = atomPtr->next;
    }
    
    atomPtr = (ATOM *)malloc(sizeof(*atomPtr));    
    memset(atomPtr , 0 , sizeof(*atomPtr));

    atomPtr->str = (char *)str;
    atomPtr->len = len;

    atomPtr->next = buskets[index];
    buskets[index] = atomPtr;

    return atomPtr->str;
}

void atomShow()
{
    assert(initBusket.buskets != NULL);
    assert(initBusket.func != NULL);

    ATOM **buskets = initBusket.buskets;
    ATOM *atomPtr = NULL;
    int counter = 0;

    int i = 0;
    for(i = 0 ; i < initBusket.busketNumbers ; ++ i)
    {
        atomPtr = buskets[i];
        if(NULL == atomPtr)
            continue;
        printf("in line %d : \n" , i + 1);
        counter = 0;

        while(atomPtr != NULL)
        {
            printf("item %d : len is %d and string is : %s  \n" , ++counter , atomPtr->len , atomPtr->str);
            atomPtr = atomPtr->next;
        }
    }
}
