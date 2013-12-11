/*
 * =====================================================================================
 *
 *       Filename:  memory.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 08:55:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Memory.h"
#include "Log/Debug.h"
#include "HashFunc.h"

#define MAX_HASH_FUNC_NUM 20
#define ERROR_RATE        0.03
#define DO_ALIGN(size , align) (((size) + (align)) & ~((align) - 1));
#define ONE_KB   (1024 * 8)

char *allocate_all_memory(unsigned long bitmap_size)
{
    unsigned long need_memory = bitmap_size;

    char *all_memory = (char *)malloc(need_memory);
    if(NULL == all_memory) 
    {
        LOG_ERROR("malloc all bitmap size %ld error!" , need_memory);
        return NULL;
    } 
    return all_memory;
}

/*  
unsigned long bloom_filter_calculate(unsigned scale , int *hash_func_num)
{
    int i = 1;
    unsigned long minBitmapSize = 0;
    int fittestHashNum = 0;

    for(i = 1 ; i < MAX_HASH_FUNC_NUM ; ++ i)    //哈希函数从1到20，计算使得位图最小的哈希函数值
    {
        double temp1 = (-1) * (scale + 0.5) * i;

        double temp2 = log(1 - pow(ERROR_RATE , ((double)1 / i)));
        unsigned long m = temp1 / temp2 + 1;

        if(1 == i)
        {
            minBitmapSize = m;
            fittestHashNum = i;
        }
        else if(m < minBitmapSize)
        {
            minBitmapSize = m;
            fittestHashNum = i;
        }
    }

    minBitmapSize = DO_ALIGN(minBitmapSize , ONE_KB);
    *hash_func_num = fittestHashNum;

    return minBitmapSize;
}
*/

void deal_with_infohash_with_all_buffer(char *bitmap_block_buffer , int bitmap_block_size , char *infohash , int length)
{
    unsigned int location = 0;
    for_each_result(infohash , length , location)
    {
        location %= bitmap_block_size;
        set_bit_in_bitmap(bitmap_block_buffer , bitmap_block_size , location);
    }
}

void set_bit_in_bitmap(char *bitmap , int length , unsigned int location)
{
    int byte_nr = location / 8;
    char *byte = bitmap + byte_nr;
    int bit_nr = location % 8;

    *byte = *byte | (1 << 8 - bit_nr - 1);
}

