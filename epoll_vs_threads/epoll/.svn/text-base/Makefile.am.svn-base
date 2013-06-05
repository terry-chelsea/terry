AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = EPOLL_SERVER

EPOLL_SERVER_SOURCES = $(top_srcdir)/src/main.cpp \
			 $(top_srcdir)/src/Task.cpp \
			 $(top_srcdir)/src/Login_task.cpp \
			 $(top_srcdir)/src/ca_login.pb.cc \
			 $(top_srcdir)/src/agent.cpp 

EPOLL_SERVER_LDADD = $(top_srcdir)/build/common/libcomm.a \
		   $(top_srcdir)/build/ThreadPool/libthread.a \
		   $(top_srcdir)/build/database/libdb.a

GLOBAL_INCLUDES = -I$(top_srcdir)/common \
				  -I$(top_srcdir)/ThreadPool \
				  -I$(top_srcdir)/database \
				  -I/usr/local/libzdb/include/zdb \
				  -I/usr/local/openssl/include/openssl 

export GLOBAL_INCLUDES

INCLUDES = $(GLOBAL_INCLUDES)

GLOBAL_LIBS = -lpthread -lprotobuf -lrt -lzdb -lcrypto -ldl
export  GLOBAL_LIBS

LIBS = -L/usr/local/libzdb/lib \
	   -L/usr/local/openssl/lib

LIBS += $(GLOBAL_LIBS)

GLOBAL_FLAGS = -g -pg -Wall -Werror -O0
CXXFLAGS += ${GLOBAL_FLAGS} -DLEVEL=1
export GLOBAL_FLAGS

SUBDIRS = common ThreadPool database
