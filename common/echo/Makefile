echo_all = echo_server echo_client client mutil_client
all : ${echo_all}
	echo "g++ works well..."
flags = -Werror -Wall -g

echo_server : server.cpp 
	g++ -o echo_server server.cpp ../libcomm.a  ${flags}

echo_client : echo_client.cpp
	g++ -o echo_client echo_client.cpp ../libcomm.a ${flags}

client : client.cpp 
	g++ -o client client.cpp  ${flags}

mutil_client : mutil_client.cpp 
	g++ -o mutil_client mutil_client.cpp ../libcomm.a ${flags}

clean : 
	rm ${echo_all}

