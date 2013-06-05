AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libthread.a

libthread_a_SOURCES = ./Mutex.cpp \
					  ./Lock.h \
					  ./Time.cpp \
					  ./util.cpp \
					  ./Thread.cpp \
					  ./ThreadPool.cpp \
					  ./WorkItem.cpp \
					  ./Dispatcher.cpp 


INCLUDES = $(GLOBAL_INCLUDES)

LIBS += $(GLOBAL_LIBS)

CXXFLAGS = $(GLOBAL_FLAG)
