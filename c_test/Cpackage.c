/*
 * =====================================================================================
 *
 *       Filename:  Cpackage.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/13 10:20:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Cpackage.h"
#include <stdlib.h>

struct point
{
    int x ;
    int y ;
};

Point *create_point(int x , int y)
{
    Point *pt = (Point *)malloc(sizeof(Point));
    pt->x = x;
    pt->y = y;

    return pt;
}

int get_x(Point *p)
{
    return p->x;
}

int get_y(Point *p)
{
    return p->y;
}
