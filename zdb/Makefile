libzdbc : Database_manager.o Database_handle.o prepare_handle.o result_handle.o
	g++ -Wall -o libzdbc.a Database_manager.o Database_handle.o prepare_handle.o result_handle.o -lzdb -I/usr/local/linzdb/include/zdb -g

Database_manager.o : Database_manager.h Database_manager.cpp
	g++ -c -g -Wall Database_manager.cpp -I/usr/local/libzdb/include/zdb

Database_handle.o : Database_handle.h Database_handle.cpp
	g++ -c -g -Wall Database_handle.cpp -I/usr/local/libzdb/include/zdb

prepare_handle.o : prepare_handle.h prepare_handle.cpp
	g++ -c -g -Wall prepare_handle.cpp -I/usr/local/libzdb/include/zdb

result_handle.o : result_handle.h result_handle.cpp
	g++ -c -g -Wall result_handle.cpp -I/usr/local/libzdb/include/zdb


clean :
	rm *.o libzdbc.a
