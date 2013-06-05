AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = THREAD_SERVER

THREAD_SERVER_SOURCES = $(top_srcdir)/src/log.cpp \
			 $(top_srcdir)/src/Header.cpp \
			 $(top_srcdir)/src/ca_login.pb.cc \
			 $(top_srcdir)/src/database.cpp \
			 $(top_srcdir)/src/operations.cpp \
			 $(top_srcdir)/src/work_func.cpp \
			 $(top_srcdir)/src/thread_server.cpp

THREAD_SERVER_LDADD = $(top_srcdir)/build/database/libdb.a

GLOBAL_INCLUDES = -I$(top_srcdir)/database \
				  -I/usr/local/libzdb/include/zdb 

export GLOBAL_INCLUDES

INCLUDES = $(GLOBAL_INCLUDES)

GLOBAL_LIBS = -lpthread -lprotobuf -lrt -lzdb -ldl
export  GLOBAL_LIBS

LIBS = -L/usr/local/libzdb/lib

LIBS += $(GLOBAL_LIBS)

GLOBAL_FLAGS = -g -pg -Wall -Werror -O0
CXXFLAGS += ${GLOBAL_FLAGS} -DLEVEL=1
export GLOBAL_FLAGS

SUBDIRS = database
