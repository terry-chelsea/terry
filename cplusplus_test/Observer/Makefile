flags = -Wall -Werror -O1 -g


observer : observer.o subject.o test.cpp
	g++ -o test test.cpp observer.o subject.o ${flags}

observer.o : observer.h observer.cpp 
	g++ -c observer.cpp  ${flags}

subject.o : subject.h subject.cpp 
	g++ -c subject.cpp ${flags}

clean :
	rm observer.o subject.o test
