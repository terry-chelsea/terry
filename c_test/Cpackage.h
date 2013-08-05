/*
 * =====================================================================================
 *
 *       Filename:  Cpackage.h
 *
 *    Description:  do infomation package use language C... 
 *
 *        Version:  1.0
 *        Created:  07/19/13 10:18:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_cpackage_h_
#define _H_cpackage_h_

typedef struct point Point;

Point *create_point(int x , int y);

int get_x(Point *p);

int gte_y(Point *p);

#endif
