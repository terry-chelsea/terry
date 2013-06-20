flags = -g -Wall -Werror

test : test.proto test_protobuf.cpp
	protoc -I. --cpp_out=. ./test.proto
	g++ -o tes_pbt test_protobuf.cpp test.pb.cc ${flags} -lprotoc

reflect : reflect_protobuf.cpp test.proto
	protoc -I. --cpp_out=. ./test.proto
	g++ -o reflect_pb reflect_protobuf.cpp test.pb.cc ${flags} -lprotoc

clean :
	rm test.pb.* test_pb reflect_pb

