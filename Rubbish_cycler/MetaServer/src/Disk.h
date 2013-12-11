/*
 * =====================================================================================
 *
 *       Filename:  Disk.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:07:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_DISK_H_
#define _H_TERRY_DISK_H_

#include "Header.h"

extern int deal_with_check_disk(int fd);

extern int check_disk_state();

extern int recheck_bucket_state();

#endif
