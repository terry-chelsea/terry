/*
 * =====================================================================================
 *
 *       Filename:  test_package.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/13 10:23:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Cpackage.h"
#include <stdio.h>

int main()
{
    Point *point = create_point(1 , 2);
    printf("x  is %d and y is %d\n" , get_x(point) , get_y(point));

    free(point);
    return 0;
}
