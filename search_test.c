
/*
 * Implemention of KMP search : 
 * find the first appearance of a given string from a long string...
 * Time complexity : O(n)...n is length of long string
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "Search.h"

#define LENGTH      2048
#define SUB_LENGTH  256

#if 1
struct timeval start , end;
int main(int argc , char *argv[])
{
    int choice = 0;
    if(argv[1] != NULL)
        choice = atoi(argv[1]);


    char long_line[LENGTH + 16];
    char line[SUB_LENGTH * 2];
    
    const char *(*Search_func)(const char * , const char *);
    const char *info = NULL;
    if(1 == choice)
    {
        Search_func = search_string;
        info = "----------With Noraal Search----------";
    }
    else  if(2 == choice)
    {
        Search_func = kmp_search_string;
        info = "----------With KMP Search----------";
    }
    else 
    {
        info = "----------With BM Search----------";
        Search_func = bm_search_string;
    }

    printf("%s\n" , info);
    int success = 0 , failed = 0 ;
    
    double sum_time = 0.0;
    while(1)
    {
        if(fgets(long_line , LENGTH + 16 , stdin) == NULL)
            break;

        int llen = strlen(long_line);
        if(long_line[llen - 1] == '\n')
            long_line[llen - 1] = '\0';


        memset(line , 0 , SUB_LENGTH * 2);
        if(fgets(line , SUB_LENGTH * 2 , stdin) == NULL)
            break;

        int len = strlen(line);
        if(line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            len --;
        }

        gettimeofday(&start , NULL);

        const char *ptr = Search_func(long_line , line);

        gettimeofday(&end , NULL);
        if(NULL != ptr)
        {
//            printf("Length %d search %d Find substring : index %ld\n" , 
//                    llen , len , ptr - long_line);
            ++ success;
        }
        else 
//            printf("Length %d search %d Can not find substring !\n" , 
//                    llen , len);
        {
            ++ failed;
        }
        double gap =  ((end.tv_sec - start.tv_sec) * 1000000 + 
                 (end.tv_usec - start.tv_usec)) / (double)1000;

//        printf("Cost time : %lfms\n" , gap);
        sum_time += gap;

//        printf("--------------------------------------------------\n");

    }

    printf("Success search %d , failed %d\n" , success , failed);
    printf("Sum up time Cost %lf ms\n" , sum_time);

    return 0;
}

#else

#include <unistd.h>
#include <fcntl.h>
#define TEST_FILE "./searchfile"

char chars[] = "abcdefghijklmnopqrstuvwxyz";
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
        int len = rand() % 8 + SUB_LENGTH - 8;
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
    
        char sub[SUB_LENGTH];
        int len = rand() % 8 + SUB_LENGTH - 8;
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
