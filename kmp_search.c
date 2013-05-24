
/*
 * Implemention of KMP search : 
 * find the first appearance of a given string from a long string...
 * Time complexity : O(n)...n is length of long string
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int compare_string(const char *start , const char *end , int len)
{
    int i = 0;
    for(i = 0 ; i < len ; ++ i)
    {
        if(start[i] != end[i])
            break;
    }

    return i == len;
}
int *create_string_table(const char *str , int len)
{
    int *table = (int *)calloc(len , sizeof(int));
    if(NULL == table)
    {
        printf("Allocate %d bytes memory failed !\n" , len * sizeof(len));
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

const char *kmp_search(char *src , char *search)
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

const char *normal_search(const char *src , const char *search)
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
const char *bm_search(const char *src , const char *search)
{
    int src_len = strlen(src);
    int len = strlen(search);
    if(len > src_len)
        return NULL;
    
#define MAX(a , b) ((a) > (b) ? (a) : (b))
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
            ret = src + k;
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
            i += MAX(step , max_gap);
    }

    return ret;
}

#define LENGTH   1000000
#define SUB_LENGTH  26

#ifndef _TEST
struct timeval start , end;
int main()
{
    char long_line[LENGTH + 16];
    char line[SUB_LENGTH * 2];
#ifdef NORMAL
    char *info = "----------With Noraal Search----------";
#elif defined KMP
    char *info = "----------With KMP Search----------";
#else
    char *info = "----------With BM Search----------";
#endif
    printf("%s\n" , info);
    
    double sum_time = 0.0;
    while(1)
    {
        if(fgets(long_line , LENGTH + 16 , stdin) == NULL)
            break;

        int llen = strlen(long_line);
        if(long_line[llen - 1] = '\n')
            long_line[llen - 1] = '\0';


        memset(line , 0 , SUB_LENGTH * 2);
        if(fgets(line , SUB_LENGTH * 2 , stdin) == NULL)
            break;

        int len = strlen(line);
        if(line[len - 1] = '\n')
        {
            line[len - 1] = '\0';
            len --;
        }

        gettimeofday(&start , NULL);

#ifdef NORMAL
        const char *ptr = normal_search(long_line , line);
#elif defined KMP
        const char *ptr = kmp_search(long_line , line);
#else 
        const char *ptr = bm_search(long_line , line);
#endif
        gettimeofday(&end , NULL);
        if(NULL != ptr)
            printf("Length %d search %d Find substring : index %d\n" , 
                    llen , len , ptr - long_line);
        else 
            printf("Length %d search %d Can not find substring !\n" , 
                    llen , len);
        double gap =  ((end.tv_sec - start.tv_sec) * 1000000 + 
                 (end.tv_usec - start.tv_usec)) / (double)1000;

        printf("Cost time : %lfms\n" , gap);
        sum_time += gap;

        printf("--------------------------------------------------\n");

    }
    printf("Sum up time Cost %lf ms\n" , sum_time);

    return 0;
}

#else

#include <unistd.h>
#include <fcntl.h>
#define TEST_FILE "./test_kmp"
#define LENGTH   1000000
#define SUB_LENGTH  26

char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int chars_len = sizeof(chars) - 1;
void get_random_string(char *buf , int len)
{
    int i = 0;
    for(i = 0 ; i < len ; ++ i)
        buf[i] = chars[rand() % chars_len];
}

int main(int argc , char *argv[])
{
    if(argc != 3)
    {
        printf("./Usage : ./test success failed_number\n");
        return -1;
    }
    int suc = atoi(argv[1]);
    int failed = atoi(argv[2]);
    srand(getpid());
    int fd = open(TEST_FILE , O_WRONLY | O_APPEND | O_TRUNC);
    if(fd < 0)
    {
         perror("Open test file failed : ");
         return -1;
    }

    char long_line[LENGTH + 1];
    int i = 0;
    for(i = 0 ; i < suc ; ++ i)
    {
        get_random_string(long_line , LENGTH);
        long_line[LENGTH] = '\n';
        int ls = rand() % (LENGTH - SUB_LENGTH);
    
        char sub[SUB_LENGTH];
        int len = rand() % 10 + SUB_LENGTH - 10;
        memcpy(sub , long_line + ls , len);
        sub[len] = '\n';

        if(write(fd , long_line , LENGTH + 1) != LENGTH + 1)
        {
            perror("write failed : ");
            return -1;
        }
        if(write(fd , sub , len + 1) != len + 1)
        {
            perror("write sub failed : ");
            return -1;
        }
    }

    for(i = 0 ; i < failed ; ++ i)
    {
        get_random_string(long_line , LENGTH);
        long_line[LENGTH] = '\n';
        int ls = rand() % (LENGTH - SUB_LENGTH);
    
        char sub[SUB_LENGTH];
        int len = rand() % 10 + SUB_LENGTH - 10;
        get_random_string(sub , len);
        sub[len] = '\n';

        if(write(fd , long_line , LENGTH + 1) != LENGTH + 1)
        {
            perror("write failed : ");
            return -1;
        }
        if(write(fd , sub , len + 1) != len + 1)
        {
            perror("write sub failed : ");
            return -1;
        }
    }

    close(fd);
    return 0;
}

#endif
