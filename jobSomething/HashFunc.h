#ifndef _H_HASH_FUNC_H_
#define _H_HASH_FUNC_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_HASH_NUMBER 20
#define STRING_LENGTH   40

typedef unsigned int RET_TYPE;
typedef RET_TYPE (* HASH_FUNC) (char * , int);

#define ALIGN_SIZE      (sizeof(RET_TYPE) * 8 - 1)
#define ERROR_RET       0
extern int __i__i__;

/* loop get each hash result , and store them to p provided by user */
#define for_each_result_in(p , handler)  \
        for(__i__i__ = 0 ; (handler != NULL) && (__i__i__ < handler->m_hashFuncNum) && ((p = (handler->m_resultArray)[__i__i__]) || 0x1) ; ++ __i__i__)
#define for_each_result(str , size , p) \
    generateNResult(str , size); \
    for_each_result_in(p , getObjHandler())

#define for_each_result_out(obj , str , size , p) \
    generateNResultOut(obj , str , size); \
    for_each_result_in(p , obj)



/* Usage : first use createHashFuncObj() function to create a HASH_FUNC_OBJ store in obj,
 * the parameter is how many hash functions you want to use.
 * then pass the string and length to calculate every result
 * you only need to create a variable and use for_each_result() to get every result and do your jobs ,
 * str means the string pointer , size means length of string and p means the variable to store every result ,
 * after use , destoryHashFuncObj() should be used to destory obj.
 */

typedef struct 
{
	int m_hashFuncNum;
	RET_TYPE m_resultArray[MAX_HASH_NUMBER];
}HASH_FUNC_OBJ;

extern HASH_FUNC_OBJ *ojb;

extern int createHashFuncObj(int);

extern HASH_FUNC_OBJ *createHashFuncObjOut(int);

HASH_FUNC_OBJ *getObjHandler();

void generateNResult(char * , int);

void generateNResultOut(HASH_FUNC_OBJ *obj , char * , int);

extern void destoryHashFuncObj();

extern void destoryHashFuncObjOut();

RET_TYPE RSHashFunc(char * , int);

RET_TYPE JSHashFunc(char * , int);

RET_TYPE PJWHashFunc(char * , int);

RET_TYPE ELFHashFunc(char * , int);

RET_TYPE SDBMHashFunc(char * , int);

RET_TYPE DJBHashFunc(char * , int);

RET_TYPE DEKHashFunc(char * , int);

RET_TYPE APHashFunc(char * , int);

RET_TYPE AP2HashFunc(char * , int);

RET_TYPE SSLHashFunc(char * , int);

RET_TYPE MYSQL1HashFunc(char * , int);

RET_TYPE MYSQL2HashFunc(char * , int);

RET_TYPE ANOHashFunc(char * , int);

RET_TYPE BKDR1HashFunc(char * , int);

RET_TYPE BKDR2HashFunc(char * , int);

RET_TYPE BKDR3HashFunc(char * , int);

RET_TYPE BKDR4HashFunc(char * , int);

RET_TYPE BKDR5HashFunc(char * , int);

RET_TYPE MY2HashFunc(char * , int);

RET_TYPE MY3HashFunc(char * , int);

RET_TYPE MY4HashFunc(char * , int);

extern HASH_FUNC hashFunc[MAX_HASH_NUMBER];

#endif
