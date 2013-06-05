all : dArray sort test.cpp
	g++ -o test test.cpp dArray.o sort.o -g -O2 -Wall

dArray : dArray.h dArray.cpp 
	g++ -c dArray.cpp -g -O2 -Wall

sort : sort.h sort.cpp
	g++ -c sort.cpp -g -O2 -Wall

clean :
	rm *.o test
