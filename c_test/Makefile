flags = -g -O0 -Wall -Werror
list : thread_test.c nlist
	gcc -o thread_test thread_test.c normal_list.o ${flags} -lpthread -DTHREADS -DATOMIC 

nlist : normal_list.h normal_list.c
	gcc -c normal_list.c ${flags} -lpthread -DDOUBLE_LOCK_

listline : listline.c nlist request
	gcc -o listline listline.c normal_list.o request.o ${flags} -lpthread -lm -DATOMIC -DDEBUG

request : request.h request.c
	gcc -c request.c ${flags} -lm 
