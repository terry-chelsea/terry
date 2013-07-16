flags = -g -O0 -Wall -Werror
all = thread_test normal_list.o listline request.o 

list : thread_test.c normal_list.o
	gcc -o thread_test thread_test.c normal_list.o ${flags} -lpthread -DTHREADS -DATOMIC 

normal_list.o : normal_list.h normal_list.c
	gcc -c normal_list.c ${flags} -lpthread -DDOUBLE_LOCK_

listline : listline.c normal_list.o request.o
	gcc -o listline listline.c normal_list.o request.o ${flags} -lpthread -lm -DATOMIC -DIN_DEBUG

request.o : request.h request.c
	gcc -c request.c ${flags} -lm 

clean :
	rm ${all}
