#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "HashFunc.h"

#define MAX_NUMBER 1000000

//使用bloom fiiler算法判断从文件中读取的数据是否在1-MAX_NUMBER之间
//根据给定的误判率，查看误判是否准确
//
#define ERROR_RATE 0.003
#define MAX_HASH_FUNC 20

#define ALIGN_TO_BYTE(n)  (((n % 8 != 0) && (n = ((n + 7) & (~0x7U)))) , n)

#define PCS 1e-10
double nsqrt(double val , unsigned int n)
{
    double low = 0 , high = val;
    if(val < 1)
        high = 1;
    double mid = (low + high) / 2;

    if(n == 1 || 1 == val)
        return val;

    double powv = pow(mid , n);
    while(powv - val > PCS || val - powv > PCS)
    {
        if(powv > val)
            high = mid;
        else 
            low = mid;

        if(high - low < PCS)
            break;
        mid = (low + high) / 2;
        powv = pow(mid , n);
    }

//    printf("pow(%lf , %d) = %lf  compare %lf\n" , mid , n , pow(mid , n) , val);
    return mid;
}

double cal_bitmap_size(unsigned long long set_size , double err_rate , int *hash)
{
    double min = set_size * 1000;
    double m = 0;
    
    int i = 0;
    for(i = 2 ; i <= MAX_HASH_FUNC ; ++ i)
    {
        double tmp = 1 - nsqrt(err_rate , i);
        m = (-1) * i * (set_size + 0.5) / log(tmp) + 1;
        if(m < min)
        {
            min = m;
            *hash = i;
        }
    }

    return min;
}

unsigned long long count = 0;
//UNbelieve BUG !!!
//在下面的宏定义中，不能使用temp作为临时变量，因为它会覆盖之前的值
//而且导致n的值总是0！！！这是为什么？！！！
//改成其他的变量就可以了
//!!!在宏定义中，一定不能使用和原函数中一样的变量名，要不然造成覆盖
//那样的BUG，会让你抓耳挠腮，痛不欲生!!!
#define SIGN_BIT(s , sz , n)   do{          \
    unsigned int __temp__ = n % (sz << 3);  \
    unsigned int index = __temp__ >> 3;     \
    unsigned int bit = __temp__ & 0x7;      \
/*                                          \
    unsigned int temp = n % (sz << 3);      \
    unsigned int index = temp >> 3;         \
    unsigned int bit = temp & 0x7;          \
    if(count ++ == 11)                      \
    printf("n =  %u\n" , n);                \
                                            \
*/                                          \
    unsigned char *chp = (unsigned char *)s;\
    chp[index] |= (1 << (8 - bit - 1));     \
}while(0)

#define GET_MOD8(n , m)  (n % (m << 3))
#define CHECK_BIT(s , sz , n) \
     (((*((unsigned char *)s + (GET_MOD8(n , sz) >> 3)) <<  \
       (GET_MOD8(n , sz) & 0x7)) & (1 << 7))) 


int check_number(char *bitmap , int sz , unsigned int num)
{
    char str[16];
    int len = snprintf(str , 16 - 1 , "%u" , num);

    unsigned int temp = 0;
    for_each_result(str , len + 1 , temp)
    {
        if(!CHECK_BIT(bitmap , sz , temp))
            return 0;
    }

    return 1;
}


void generate_bitmap(char *bitmap , int sz)
{
    char str[16];
    int i = 0;
    unsigned int temp = 0;
    for(i = 10 ; i <= MAX_NUMBER ; ++ i) 
    {
        int len = snprintf(str , 16 - 1 , "%d" , i);
        for_each_result(str , len + 1 , temp)
        {
            if(count == 11)
                printf("temp = %u\n" , temp);
            SIGN_BIT(bitmap , sz , temp);
        }
    }

#if 0
#define OUT_FILE "bitmap_file"
    int fd = open(OUT_FILE , O_CREAT | O_TRUNC | O_WRONLY , 0x666);
    if(fd < 0)
    {
        perror("creat file bitmap file failed : ");
        return ;
    }

    if(write(fd , bitmap , ret) != ret)
    {
        perror("Write bitmap to file failed : ");
        goto RET;
    }
    
    if(fsync(fd) < 0)
    {
        perror("file sync failed : ");
        goto RET;
    }

RET :
    close(fd);
#endif

}

unsigned int __failed_tests = 0;
unsigned int __test_num = 0;
#define test_cond(descr,_c) do { \
    __test_num++; printf("%d - %s: ", __test_num, descr); \
    if(_c) printf("PASSED\n"); else {printf("FAILED\n"); __failed_tests++;} \
} while(0)
#define test_report() do { \
    fprintf(stderr , "%d tests, %d passed, %d failed\n", __test_num, \
                    __test_num-__failed_tests, __failed_tests); \
    if (__failed_tests) { \
        fprintf(stderr , "=== WARNING === We have failed tests here...\n"); \
        exit(1); \
    } \
} while(0)


#define MAX_VALUE 0xFFFFFFFFU
#define IS_SUCCEED(num) ((num >= 10) && (num <= MAX_NUMBER))
//如果a为真，b为真，返回真
//如果a为假，b为假，返回真
//否则返回假
#define DECIDE(a , b)
void start_test(char *bm , int sz)
{
    char str[64];
    while(1)
    {
        if(fgets(str , 64 , stdin) != NULL)
        {
            int len = strlen(str);
            if(str[len - 1] == '\n')
                str[-- len] = '\0';

            unsigned long long input = atoll(str);
            if(input <= 0 || input >= MAX_VALUE)
                continue ;

            snprintf(str , 32 , "test %lld" , input);

            int ret = IS_SUCCEED(input);
            int check_ret = check_number(bm , sz , input);
            if(ret)
            {
                if(!check_ret)
                    fprintf(stderr , "Never Happened !!! Number %d FAILED !\n" , input);
                test_cond("str" , check_ret);
            }
            else 
            {
                if(check_ret)
                    fprintf(stderr , "Number %d FAILED !\n" , input);
                test_cond("str" , !check_ret);
            }

        }
        else 
            break;
    }

    test_report();
}


int test_bloom_filter()
{
    int hash = 0;
//    printf("Min bitmap size is %lf bits , hash functions %d\n" , m , hash);
    double m = cal_bitmap_size(MAX_NUMBER , ERROR_RATE , &hash);
    unsigned long long ret = (unsigned long long)m;
    ret = ALIGN_TO_BYTE(ret) / 8;;

    char *bitmap = (char *)calloc(ret , sizeof(char));
    if(NULL == bitmap)
    {
        printf("Allocate %d bytes memory failed !\n" , ret);
        return -1;
    }

    if(createHashFuncObj(hash) < 0)
    {
        printf("Create object error !");
        return -1;
    }

    struct timeval start , end;
    gettimeofday(&start , NULL);
    generate_bitmap(bitmap , ret);
    gettimeofday(&end , NULL);

    printf("generate bitmap cost %lfms\n" , 
            ((end.tv_sec - start.tv_sec) * 1000000 + 
             (end.tv_usec - start.tv_usec)) / (double)1000);

    start_test(bitmap , ret);

    return 0;
}


int test_set_bitmap()
{
    unsigned int a[10];
    memset(a , 0 , sizeof(a));

    int i = 0;
    for(i = 0 ; i < sizeof(a) * 8 ; i += 8)
    {
        SIGN_BIT(&a , sizeof(a) , i);
        printf("sign bit %d result %0X\n" , i , a);
    }

    for(i = 0 ; i < sizeof(a) * 8 ; i += 2)
    {
        if(CHECK_BIT(&a , sizeof(a) , i))
            printf("bit %d set !\n" , i);
        else 
            printf("bit %d not set !\n" , i);
    }

    return 0;
}


#define GENERATE_FILE "./bmtest"
#include <time.h>

int generate_test_numbers(char *cnts)
{
    srand(time(NULL));

    int fd = open(GENERATE_FILE , O_WRONLY | O_CREAT | O_TRUNC | O_APPEND , 0644);
    if(fd < 0)
    {
        perror("open generate file failed !\n");
        return -1;
    }

    char str[32];
    int i = 0;
    int counter = atoi(cnts);
    for(i = 0 ; i < counter ; ++ i)
    {
        int rdm = rand();
        int len = snprintf(str , 32 - 1 , "%d" , rdm);
        str[len - 1] = '\n';
        if(write(fd , str , len) != len)
        {
            perror("write random data to file failed : ");
            goto RET;
        }
    }

RET :
    close(fd);
    return 0;
}


int main(int argc , char *argv[])
{
    int choice = 0;
    if(argc > 1)
        choice = atoi(argv[1]);

    switch(choice)
    {
        case 1 : 
            test_set_bitmap();
            break;
        case 2 :
            if(argc < 3)
            {
                printf("./Usage : ./test counter\n");
                break;
            }
            generate_test_numbers(argv[2]);
            break;

        default :
            test_bloom_filter();
            break;
    }

    printf("DONE!!!\n");
    return 0;
}
