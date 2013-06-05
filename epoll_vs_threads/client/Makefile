target = client
a_include = ./common/
LIB_ = ./common/libcomm.a

client : client.cpp protocol.h ca_login.pb.o ${LIB_}
	g++ -o client client.cpp ca_login.pb.o ${LIB_} -I${a_include} -g -Wall -Werror -lprotobuf


ca_login.pb.o :  ca_login.proto
	g++ -c  ca_login.pb.cc

clean : 
	rm ${target}  ca_login.pb.o
