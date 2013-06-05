flags = -O0 -Werror -Wall -g
targets = singleton

all : Singleton
	echo "Make all targets..."

Singleton : Singleton/Singleton.cpp
	g++ -o singleton Singleton/Singleton.cpp ${flags} -lpthread

clean : 
	rm ${targets}
