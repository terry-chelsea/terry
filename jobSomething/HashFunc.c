#include "HashFunc.h"

HASH_FUNC_OBJ *obj = NULL;

HASH_FUNC hashFunc[MAX_HASH_NUMBER] = {
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

int __i__i__ = 0;

RET_TYPE RSHashFunc(char *str , int size)
{
	int b = 378551;
	int a = 63689;
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = hash * a + str[i];
		a = a * b;
	}
	return hash;
}

RET_TYPE JSHashFunc(char * str , int size)
{
	RET_TYPE hash = 1315423911;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash ^= ((hash << 5) + str[i] + (hash >> 2));
	}
	return hash;
}

RET_TYPE PJWHashFunc(char *str , int size)
{
	unsigned int bitInUnsignedInt = 4 * 8;	
	unsigned int threeQuarters = (bitInUnsignedInt * 3) / 4;
	unsigned int oneEight = bitInUnsignedInt / 8;
	unsigned int highBits = 0xFFFFFFFF << (bitInUnsignedInt - oneEight);
	
	RET_TYPE hash = 0;
	RET_TYPE test = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = (hash << oneEight) + str[i];
		if((test = hash & highBits) != 0)
		{
			hash = ((hash ^ (test >> threeQuarters)) & (~highBits));
		}
	}
	return hash;
}

RET_TYPE ELFHashFunc(char * str , int size)
{
	RET_TYPE hash = 0;
	unsigned int x = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = (hash << 4) + str[i];
		if((x = hash & 0xF0000000) != 0)
		{
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}
	return hash;
}

RET_TYPE BKDRHashFunc(char * str , int size , int seed)
{
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = (hash * seed) + str[i];
	}
	return hash;
}

RET_TYPE BKDR1HashFunc(char * str , int size)
{
	return BKDRHashFunc(str , size , 131);
}

RET_TYPE BKDR2HashFunc(char * str , int size)
{
	return BKDRHashFunc(str , size , 1313);
}

RET_TYPE BKDR3HashFunc(char * str , int size)
{
	return BKDRHashFunc(str , size , 13131);
}

RET_TYPE BKDR4HashFunc(char * str , int size)
{
	return BKDRHashFunc(str , size , 131313);
}

RET_TYPE BKDR5HashFunc(char * str , int size)
{
	return BKDRHashFunc(str , size , 1313131);
}

RET_TYPE SDBMHashFunc(char * str , int size)
{
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = str[i] + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

RET_TYPE DJBHashFunc(char * str , int size)
{
	RET_TYPE hash = 5381;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = ((hash << 5) + hash) + str[i];
	}
	return hash;
}

RET_TYPE DEKHashFunc(char * str , int size)
{
	RET_TYPE hash = size;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = ((hash << 5) ^ (hash >> 27)) ^ str[i];
	}
	return hash;
}

RET_TYPE AP(char * str , int size , RET_TYPE init)
{
	RET_TYPE hash = init;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		if((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ str[i] * (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) + ((str[i]) ^ (hash >> 5))));	
		}
	}
	return hash;
}

RET_TYPE APHashFunc(char *str , int size)
{
	return AP(str , size , 0xAAAAAAAA);			
}

RET_TYPE AP2HashFunc(char *str , int size)
{
	return AP(str , size , 0x16FDECA7);
}

RET_TYPE SSLHashFunc(char * str , int size)
{
	int halfSize = (size + 1) / 2;
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < halfSize ; ++ i)
	{
		hash ^= (str[i] << (i & 0x0F));	
	}
	return hash;
}

RET_TYPE MYSQL1HashFunc(char * str , int size)
{
	RET_TYPE hash = 1;
	unsigned int seed = 4;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash ^= (((hash & 63) + seed) * str[i] + (hash << 8));
		seed += 3;
	}	
	return hash;
}

RET_TYPE MYSQL2HashFunc(char * str , int size)
{
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash *= 16777619;
		hash ^= (RET_TYPE)str[i];
	}
	return hash;
}

RET_TYPE ANOHashFunc(char * str , int size)
{
	RET_TYPE hash = 0;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash = 31 * hash + str[i];
	}
	return hash;
}

RET_TYPE MY1HashFunc(char * str , int size)
{
	RET_TYPE hash = 1;
	int quarterSize = size / 4;
	int offset = size % 4;
	unsigned int *ptr = (unsigned int *)str;
	int i = 0;
	for(i = 0 ; i < quarterSize ; ++ i)
	{
		hash ^= *ptr;
		++ ptr;
	}
	str = (char *)ptr;
	for(i = 0 ; i < offset ; ++ i)
	{
		hash ^= *str;
		++ str;	
	}
	return hash;
}

RET_TYPE MY2HashFunc(char * str , int size)
{
	RET_TYPE hash = 1316271;
	int i = 0;
	for(i = 0 ; i < size ; ++ i)
	{
		hash ^= (str[i] << (i & ALIGN_SIZE)); 
	}
	return hash;
}

RET_TYPE MY3HashFunc(char * str , int size)
{
	RET_TYPE hash = 13131;
	int i = 0;
	for(i = 0 ; i < size - 1 ; ++ i)
	{
		hash ^= str[i] << (str[i + 1] & ALIGN_SIZE);
	}
	return hash;
}

RET_TYPE MY4HashFunc(char * str , int size)
{
	RET_TYPE hash = 13131313;
	int i = 0;
	for(i = size - 1 ; i > 0 ; -- i)
	{
		hash ^= str[i] << (str[i - 1] & ALIGN_SIZE);
	}
	return hash;
}

int createHashFuncObj(int k)
{
    if(obj != NULL)
        return 0;

	if(k <= 0)
	{
		k = 1;
	}
	if(k > MAX_HASH_NUMBER)
	{
		k = MAX_HASH_NUMBER;
	}
	obj = (HASH_FUNC_OBJ *)malloc(sizeof(HASH_FUNC_OBJ));
	if(NULL == obj)
		return -1;		
	memset(obj , 0 , sizeof(*obj));
	
	obj->m_hashFuncNum = k;	
	return 0;
}

HASH_FUNC_OBJ *createHashFuncObjOut(int k)
{
    HASH_FUNC_OBJ *outObj = (HASH_FUNC_OBJ *)malloc(sizeof(HASH_FUNC_OBJ));
    if(NULL == outObj)
    {
        printf("createHashFuncObjOut() ERROR : malloc error\n");
        return NULL;
    }
	memset(outObj , 0 , sizeof(*outObj));

	if(k <= 0)
	{
		k = 1;
	}
	if(k > MAX_HASH_NUMBER)
	{
		k = MAX_HASH_NUMBER;
	}
	
	outObj->m_hashFuncNum = k;	
	return outObj;

}

HASH_FUNC_OBJ *getObjHandler()
{
	return obj;
}

void generateNResult(char *str , int size)
{
	if(NULL == obj)
	{
		printf("generateNResult() ERROR : create object first\n");
		return ;		
	}
    int flag = 0;
    if(NULL == str)
    {
        printf("generateNResult() ERROR : error string\n");
        flag = 1;
    }
	int i = 0;
	int k = obj->m_hashFuncNum;
	for(i = 0 ; i < k ; ++ i)
	{
        if(flag != 0)
        {
            (obj->m_resultArray)[i] = ERROR_RET;
        }
        else
        {
		    (obj->m_resultArray)[i] = hashFunc[i](str , size);
        }    
//		printf("time %d : result is %u\n" , i + 1 , (obj->m_resultArray)[i]);
	}
   
	return ;
}

void generateNResultOut(HASH_FUNC_OBJ *outObj , char *str , int size)
{
	if(NULL == outObj)
	{
		printf("generateNResultOut() ERROR : create object first\n");
		return ;		
	}
    int flag = 0;
    if(NULL == str)
    {
        printf("generateNResultOut() ERROR : error string\n");
        flag = 1;
    }
	int i = 0;
	int k = outObj->m_hashFuncNum;
	for(i = 0 ; i < k ; ++ i)
	{
        if(flag != 0)
        {
            (outObj->m_resultArray)[i] = ERROR_RET;
        }
        else
        {
		    (outObj->m_resultArray)[i] = hashFunc[i](str , size);
        }    
//		printf("time %d : result is %u\n" , i + 1 , (outObj->m_resultArray)[i]);
	}
   
	return ;

}

void destoryHashFuncObj()
{
	if(NULL == obj)
	{
		printf("generateNResult ERROR : create object first\n");
		return ;		
	}	
	free(obj);
	obj = NULL;
}

void destoryHashFuncObjOut(HASH_FUNC_OBJ *outObj)
{
	if(NULL == outObj)
	{
		printf("generateNResultOut() ERROR : create object first\n");
		return ;		
	}	
	free(outObj);
	outObj = NULL;

}

