libzdbc : Database_manager.o Database_handle.o prepare_handle.o result_handle.o
	g++ -shared -Wall -o libzdbc.so Database_manager.o Database_handle.o prepare_handle.o result_handle.o -lzdb -L/usr/local/libzdb/lib -I/usr/local/libzdb/include/zdb -g

Database_manager.o : Database_manager.h Database_manager.cpp
	g++ -fPIC -c -g -Wall Database_manager.cpp -I/usr/local/libzdb/include/zdb

Database_handle.o : Database_handle.h Database_handle.cpp
	g++ -fPIC -c -g -Wall Database_handle.cpp -I/usr/local/libzdb/include/zdb

prepare_handle.o : prepare_handle.h prepare_handle.cpp
	g++ -fPIC -c -g -Wall prepare_handle.cpp -I/usr/local/libzdb/include/zdb

result_handle.o : result_handle.h result_handle.cpp
	g++ -fPIC -c -g -Wall result_handle.cpp -I/usr/local/libzdb/include/zdb


clean :
	rm *.o libzdbc.so
