/*
 * Implemention of three search functions : 
 * find the first appearance of a given string from a long string...
 * Time complexity : O(n)...n is length of long string
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Search.h"

static int compare_string(const char *start , const char *end , int len)
{
    int i = 0;
    for(i = 0 ; i < len ; ++ i)
    {
        if(start[i] != end[i])
            break;
    }

    return i == len;
}

#if 0
int *create_string_table(const char *str , int len , int* table)
{
    int *table = (int *)calloc(len, sizeof(int));
    if(NULL == table)
    {
        printf("Allocate %ld bytes memory failed !\n" , len * sizeof(len));
        return NULL;
    }

    int i = 0 , j = 0;
    table[0] = 0;
    for(i = 1 ; i < len ; ++ i)
    {
        int max = 0;
        const char *end = str + i;
        for(j = 0 ; j < i ; ++ j)
        {
            if(compare_string(str , end - j , j + 1) && j + 1 > max)
            {
                max = j + 1;
            }
        }
        table[i] = max;
    }

    return table;
}
#endif

void create_string_table(const char *src , int len , int *tb)
{
    tb[0] = 0;
    int i = 0;
    int index = 0;
    for(i = 1 ; i < len ; ++ i)
    {
        index = tb[i - 1];
        while(index >= 1 && src[i] != src[index])
            index = tb[index - 1];
        if(src[i] == src[index])
            tb[i] = index + 1;
        else 
            tb[i] = 0;
    }
}

#if 0
const char *kmp_search_string(const char *src , const char *search)
{
    int src_len = strlen(src);
    int len = strlen(search);
    if(len > src_len)
        return NULL;

    int *tb = create_string_table(search , len);
    int i = 0 , j = 0;
    int limit = src_len - len + 1;

    const char *ret = NULL;
    while(i < limit)
    {
        for(j = 0 ; j < len ; ++ j)
        {
            if(src[i + j] != search[j])
                break;
        }
        if(j == len)
        {
            ret = src + i;
            break;
        } 
        if(0 == j)
            ++ i;
        else 
            i += j - tb[j - 1];
    }

    free(tb);
    return ret;
}
#endif

const char *kmp_search_string(const char *src , const char *search)
{
    int src_len = strlen(src);
    int len = strlen(search);
    if(len > src_len)
        return NULL;

    int tb[len];
    create_string_table(search , len , tb);
    int i = 0 , j = 0 , flag = 0;
    while(i < src_len && j < len)
    {
        if(src[i] == search[j])
        {
            ++ i;
            ++ j;
            flag = 1;
        }
        else if(flag)
        {
            if(!tb[j - 1])
                flag = 0;
            j = tb[j - 1];
        }
        else 
        {
            j = 0;
            ++ i;
        }
    }
    
    if(j == len)
        return src + i - len;
    else 
        return NULL;
}

const char *search_string(const char *src , const char *search)
{
    int src_len = strlen(src);
    int len = strlen(search);
    if(len > src_len)
        return NULL;
    
    const char *ret = NULL;
    int limit = src_len - len - 1;
    int i = 0;
    for(i = 0 ; i < limit ; ++ i)
    {
        if(compare_string(src + i , search , len))
        {
            ret = src + i; 
            break;
        }
    }

    return ret;
}

//如果要高效得实现该算法，需要创建查询表
//该表用于在查询查询到search的时候当出现不匹配的时候
//例如src中的字符时m,search表中字符为n, m != n
//如何能够快速找到m在search中在字符n之前第一次出现时距离n的长度
//如果不在search中出现，那么这个长度等于n在search的下标+1
//如果实现这个表！！！
//最笨的方式，就是在查询到m的时候遍历search，直到第一次出现m
//好吧，暂时先使用这个最笨的方法实现一下
const char *bm_search_string(const char *src , const char *search)
{
    int src_len = strlen(src);
    int len = strlen(search);
    if(len > src_len)
        return NULL;
    
#define MIN(a , b) ((a) < (b) ? (a) : (b))
    //这一个表用来保存每个字符是否在search中出现
    unsigned char bitmap[256] = {0};

    int i = 0 , j = 0 , k = 0;
    int max_gap = 0;
    unsigned char end = search[len - 1];
    for(i = len - 1 ; i >= 0 ; -- i)
    {
        unsigned char ch = search[i];
        bitmap[ch] += 1;
        if(ch == end)
            max_gap = len - 1 - i;
    }
    if(max_gap == 0)
        max_gap = len;
    
    i = len - 1;
    const char *ret = NULL;
    while(i < src_len)
    {
        for(j = len - 1 , k = i ; j >= 0 ; -- j , -- k) 
        {
            if(src[k] != search[j])
                break;
        }
        if(-1 == j)
        {
            ret = src + k + 1;
            break;
        }
        unsigned char ch = src[k];
        int step = 0;
        //这个不相同的字符不在search中
        if(0 == bitmap[ch])
            step = j + 1;
        else 
        {
            step = 1;
            for(k = j - 1 ; k >= 0 ; k --)
            {
                if(search[k] == ch)
                    break;
                ++ step ;
            }
        }
        if(j == len - 1)
            i += step;
        else 
            i += MIN(step , max_gap);
    }

    return ret;
}

