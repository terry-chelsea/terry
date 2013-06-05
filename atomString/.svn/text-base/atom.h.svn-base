/*
 * =====================================================================================
 *
 *       Filename:  atom.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/04/12 01:35:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_ATOM_H
#define _H_TERRY_ATOM_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<assert.h>

#define MAX_BUSKET      10000
#define DEFAULT_BUSKET  1024
#define MIN_BUSKET      128
#define MAX_NUMBER_SIZE 100
#define MAX_NUMBER      (~(1 << (8 * sizeof(int) - 1)))
#define ELEMENTS(x)          (sizeof(x) / sizeof((x)[0]))

typedef struct Atom
{
    struct Atom *next;
    unsigned long hashValue;           // just test if it can make find fast...
    int   len;
    char *str;
}ATOM;

typedef unsigned long (*HASH_FUNC)(const char * , int);

typedef struct Busket
{
    ATOM **buskets;
    int    busketNumbers;
    HASH_FUNC func;
}BUSKET;

static BUSKET initBusket;

ATOM *atomFind(const char *str , ATOM **prev);

unsigned long getHash(const char *str , int len);

extern void atomInit(int hint);

extern int atomLength(const char *str);

extern const char *atomNew(const char *str , int len);

extern const char *atomString(const char *str);

extern const char *atomInt(long value);

extern int atomFree(char *str);

extern void atomReset();

extern void atomVload(const char *str , ...);

extern void atomAload(const char *str[]);

extern const char * atomAdd(const char *str , int len);

extern void atomShow();

#endif
