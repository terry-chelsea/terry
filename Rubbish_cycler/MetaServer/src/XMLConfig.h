/*
 * =====================================================================================
 *
 *       Filename:  XMLConfig.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 05:41:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _TERRY_XML_CONFIG_H_
#define _TERRY_XML_CONFIG_H_

#define DIR_LENGTH 64
#define IP_LENGTH  32
#define BITMAP_BLOCK_PREFIX  8                        //以头八个字节分块

typedef struct 
{
    int   heartbeat_interval;
    int   check_disk_interval;
    char  NFS_dir[DIR_LENGTH];
    int   out_type;
    int   all_memory_size;
    char  CS_IP[IP_LENGTH];
    short MU_listen_fd;
    short Alive_port;
}XMLConfig;

extern XMLConfig xml_config;

extern int read_config(char *file_name);

extern void show_config();

#endif
