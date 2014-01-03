AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = comm/Agent.cpp \
					 comm/AgentManager.cpp \
					 comm/BaseTask.cpp \
					 comm/BufferManager.cpp \
					 comm/Epoll.cpp \
					 comm/EpollEvent.cpp \
					 comm/SocketAddress.cpp \
					 comm/TCPAgent.cpp \
					 comm/TCPSocket.cpp \
					 comm/Tool.cpp \
					 comm/UDPAgent.cpp \
					 comm/UDPSocket.cpp \
					 DevLog/DevLog.cpp \
					 DevLog/FunctionTrace.cpp \
					 log/Appender.cpp \
					 log/ConsoleAppender.cpp \
					 log/Logger.cpp \
					 log/LoggerFactory.cpp \
					 xml/XMLConfig.cpp \
					 util/io.cpp \
					 util/os.cpp \
					 util/os_path.cpp \
					 sys/Cond.cpp \
					 sys/Exception.cpp \
					 sys/Mutex.cpp \
					 sys/Thread.cpp \
					 sys/ThreadPool.cpp \
					 sys/Time.cpp \
					 protocol/cstore.protocol.pb.cc

INCLUDES = -I. -I/usr/local/include -I/usr/include/libxml2

LIBS += $(GLOBAL_LIBS)

CXXFLAGS += -g -pg -Wall -O0
