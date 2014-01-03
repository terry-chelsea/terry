AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = CS_RS
CS_RS_SOURCES = $(top_srcdir)/src/main.cpp

CS_RS_LDADD = $(top_srcdir)/build/src/libCS.a \
			  $(top_srcdir)/build/common/libcommon.a

GLOBAL_INCLUDES = -I$(top_srcdir)/include \
		   -I/usr/local/include \
		   -I/usr/include/libxml2 \
		   -I/usr/local/libzdb/include/zdb
export GLOBAL_INCLUDES

INCLUDES = $(GLOBAL_INCLUDES)

GLOBAL_LIBS = -lpthread -lxml2 -lprotobuf -lrt -lzdb
export GLOBAL_LIBS

LIBS = -L/usr/local/libzdb/lib

CXXFLAGS += -g -pg -Wall -O0

LIBS += $(GLOBAL_LIBS)

SUBDIRS = common src
