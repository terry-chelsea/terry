object = Agent.o AgentManager.o BaseTask.o BufferManager.o Epoll.o EpollEvent.o Exception.o SocketAddress.o TCPAgent.o TCPSocket.o Tool.o TCPListenAgent.o Server.o util.o Client.o

flags = -O2 -Werror -Wall 
target = libcomm.a

comm : ${object}
	ar cqs ${target} ${object}

Agent.o : Agent.h Agent.cpp
	g++ -c Agent.cpp ${flags}

AgentManager.o : AgentManager.h AgentManager.cpp
	g++ -c AgentManager.cpp ${flags}

BaseTask.o : BaseTask.h BaseTask.cpp
	g++ -c BaseTask.cpp ${flags}

BufferManager.o : BufferManager.h BufferManager.cpp
	g++ -c BufferManager.cpp ${flags}

Epoll.o : Epoll.h Epoll.cpp
	g++ -c Epoll.cpp ${flags}

EpollEvent.o : EpollEvent.h EpollEvent.cpp
	g++ -c EpollEvent.cpp ${flags}

Exception.o : Exception.h Exception.cpp
	g++ -c Exception.cpp ${flags}

SocketAddress.o : SocketAddress.h SocketAddress.cpp
	g++ -c SocketAddress.cpp ${flags}

TCPAgent.o : TCPAgent.h TCPAgent.cpp Agent.h
	g++ -c TCPAgent.cpp ${flags}

TCPSocket.o : TCPSocket.h TCPSocket.cpp 
	g++ -c TCPSocket.cpp ${flags}

Tool.o : Tool.cpp Tool.h
	g++ -c Tool.cpp ${flags}

TCPListenAgent.o : TCPListenAgent.h TCPListenAgent.cpp
	g++ -c TCPListenAgent.cpp ${flags}

Server.o : Server.h Server.cpp
	g++ -c Server.cpp ${flags}

Client.o : Client.cpp Client.h
	g++ -c Client.cpp ${flags}

util.o : util.cpp
	g++ -c util.cpp ${flags}

clean :
	rm ${object}  ${target}



