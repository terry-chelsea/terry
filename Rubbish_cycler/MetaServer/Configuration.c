/*
 * =====================================================================================
 *
 *       Filename:  Configuration.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/08/13 11:18:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "Debug.h"
#include <stdlib.h>
#include "Configuration.h"

struct config
{
    short listen_port;
    char *root_dir;
    char *nfs_root;

    int   is_compress;
    int  hash_number;
    int *hash_ids;
    unsigned char *bitmap;
    unsigned int  bitmap_size;
    int  infohash_blocks;
    
    int  comm_fd;
    int  check_interval;

    int  exit_flag;
};

struct config *all_config_info = NULL;

int parse_config_file(const char *file_name)
{

}
