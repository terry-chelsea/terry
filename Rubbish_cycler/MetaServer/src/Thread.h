/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 17:36:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _TERRY_THREAD_H_
#define _TERRY_THREAD_H_

#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<dirent.h>
#include<fcntl.h>
#include "Header.h"


extern GLOBLE_VALUE g_global_value;

static NODE *append_list = NULL;                 //添加的新它桶号加入到这个链表中
static NODE *append_list_backup = NULL;          //遍历的时候将新加入的所有的桶号转移到这个链表，清空前一个链表

extern int create_thread();

void *thread_run(void *);

int do_thread_work();

int do_every_bucket(NODE *head);

int read_start_cmd(int );

int traverse_bucket(int bucket_nr);

int finish_one_bucket(int bucket_nr);

int deal_with_regular_file(char *name);

int real_deal_with_regular_file(char *name);

int get_file_length(int fd);

void thread_error_exit();

void thread_ok_exit();

void generate_bucket_dir_name(char * , int , int);

int get_append_bucket(int);

#endif
