target = Timer.a 
objects = Timer.o Heap.o Timer_manager.o
flags = -g -Wall -Werror

sum : all test timer_test 

all : ${objects} 
	ar rv ${target} ${objects}

Timer.o : Timer.h Timer.cpp
	g++ -c Timer.cpp ${flags}

Heap.o : Timer.o Heap.h Heap.cpp
	g++ -c Heap.cpp ${flags}

Timer_manager.o : Timer_manager.h Timer_manager.cpp Timer.o Heap.o
	g++ -c Timer_manager.cpp ${flags}

test : test.cpp ${objects}
	g++ -o test test.cpp ${target} ${flags}

timer_test : timer_test.cpp ${objects}
	g++ -o timer_test timer_test.cpp ${target} ${flags}


clean :
	rm ${target} ${objects} test timer_test
