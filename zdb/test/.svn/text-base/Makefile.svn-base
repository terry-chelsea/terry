simple : simple_test.cpp 
	g++ -o simple simple_test.cpp -I/usr/local/include/zdb -L.. -lzdbc -g

select : select_test.cpp
	g++ -o select select_test.cpp -I/usr/local/include/zdb -L.. -lzdbc -g

insert : insert_to_yu.cpp
	g++ -o insert insert_to_yu.cpp -I/usr/local/include/zdb -L.. -lzdbc -g

transaction : transaction.cpp
	g++ -o transaction transaction.cpp -I/usr/local/include/zdb -L.. -lzdbc -g

mutil_transaction : mutil_transaction.cpp
	g++ -o mutil_transaction mutil_transaction.cpp -I/usr/local/include/zdb -L.. -lzdbc -lpthread -g

clean : 
	rm mutil_transaction  transaction insert select  simple
