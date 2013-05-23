#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NUMBER 1000000

//使用bloom fiiler算法判断从文件中读取的数据是否在1-MAX_NUMBER之间
//根据给定的误判率，查看误判是否准确
//
#define ERROR_RATE 0.001
#define MAX_HASH_FUNC 20

#define ALIGN_TO_BYTE(n)  ((n % 8 != 0) && (n = (n + 7) & 0x7))

double cal_bitmap_size(uint64_t set_size , double err_rate , int *hash)
{
    double min = set_size * 1000;
    
    int i = 0;
    for(i = 2 ; i <= MAX_HASH_FUNC ; ++ i)
    {
        double tmp = 1 - log(p) / log(i);

    }
}

