/*
 * =====================================================================================
 *
 *       Filename:  memory.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 08:50:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _TERRY_MEMORY_H_
#define _TERRY_MEMORY_H_

#include<stdlib.h>
#include<string.h>
#include<stdlib.h>

#define FILE_BLOCK 512 * 1024

extern char* allocate_all_memory(unsigned long bitmap_size);

//static unsigned long bloom_filter_calculate(unsigned scale , int *hash_func_num);

extern void deal_with_infohash_with_all_buffer(char *bitmap_block_buffer , int bitmap_block_size , char *infohash , int length);

static void set_bit_in_bitmap(char *bitmap , int length , unsigned int location);

#endif
