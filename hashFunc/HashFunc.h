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

/* loop get each hash result , and store them to p provided by user */
#define for_each_result_in(p , handler)  \
        for(int i = 0 ; (handler != NULL) && (i < handler->m_hashFuncNum) && ((p = (handler->m_resultArray)[i]) || 0x1) ; ++ i)
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

static HASH_FUNC_OBJ *ojb;

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

static HASH_FUNC hashFunc[MAX_HASH_NUMBER] = {
    //12 : 17909.400 : 2.303
	ANOHashFunc ,
    //4 : 17884.600 : 2.479
	BKDR1HashFunc ,
    //1 : 17948.200 : 2.765
	JSHashFunc , 
    //15 : 17956.400 : 2.434
	BKDR4HashFunc ,
    //6 : 17985.600 : 2.465
	DJBHashFunc ,
    //3 : 17875.000 : 3.217
	ELFHashFunc ,
    //5 : 17974.000 : 2.459
	SDBMHashFunc ,
    //16 : 18017.600 : 2.340
	BKDR5HashFunc ,
    //8 : 17966.400 : 2.644
	APHashFunc ,
    //14 : 18056.000 : 2.436
	BKDR3HashFunc ,
    //13 : 18100.800 : 2.434
	BKDR2HashFunc ,
    //7 : 18020.400 : 2.216
	DEKHashFunc ,
    //11 : 18030.200 : 2.420
	MYSQL2HashFunc ,
    //17 : 18035.000 : 2.282
	MY2HashFunc ,
    //0 : 18015.000 : 2.786
	RSHashFunc ,    
    //2 : 18028.800 : 2.645
	AP2HashFunc ,
    //10 : 21657.000 : 2.491
	MYSQL1HashFunc ,
    //9 : 32744.000 : 2.029
	SSLHashFunc ,
    //18 : 19548.200 : 2.590
	MY3HashFunc ,
    //19 : 19592.000 : 2.678
	MY4HashFunc	
};

#endif
