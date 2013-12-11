/*
 * =====================================================================================
 *
 *       Filename:  Traverse_filesystem.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:18:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_TRAVERSE_FILESYSTEM_H_
#define _H_TERRY_TRAVERSE_FILESYSTEM_H_

#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<dirent.h>
#include<fcntl.h>

#define PATH_LENGTH  1024
#define INFOHASH_LENGTH    40
#define BLOCK_SIZE         (512 * 1024)

typedef int (*DEAL_WITH_FILE)(char *);

extern char full_path[PATH_LENGTH];

extern int traverse_top_dir(char *dir_name , DEAL_WITH_FILE func);

static int traverse_second_dir(char *user_dir_name , DEAL_WITH_FILE func);

static int recursive_traverse_dir(char *dir_name , int max_path_length , DEAL_WITH_FILE func);

static int append_sub_name(char *src_dir_name , char *sub_dir_name , int max_length);

static void clear_last_dir_name(char *src_dir_name , int last_slash);

#endif
