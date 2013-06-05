flags = -Werror -Wall -g
obj = testAtom
test_atom : testAtom.c atom.o
	g++ -o ${obj} testAtom.c atom.o ${flags}

atom.o : atom.h atom.cpp
	g++ -c atom.cpp ${flags}

clean :
	rm atom.o ${obj}
