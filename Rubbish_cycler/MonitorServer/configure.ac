#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([CS_RS], [1.0], [fengyuatad@126.com])
AC_CONFIG_SRCDIR([include/ThreadPool.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(CS_RS , 1.0)

AM_PATH_XML2( , , AC_MSG_ERROR(CS_RS 1.0 need libxml2))

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
#need next when use static library..
AC_PROG_RANLIB   
#need next when use dynamic library...
#AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([protobuf] , [_ZTVN6google8protobuf17FileDescriptorSetE] , [echo "find protobuf"] , [echo "can not find protobuf" && exit 1])

AC_CHECK_LIB([xml2] , [xmlXPtrEval] , [echo "find libxml2"] , [echo "can not find libxml2" && exit 2])

AC_CHECK_LIB([rt] , [clock_gettime] , [echo "find realtime"] , [echo "can not fine realtime" && exit 1])

AC_CHECK_LIB([ptrhread] , [pthread_create] , [echo "find pthread" , [echo "can not find pthread" && exit 1]])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h sys/vfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MKTIME
AC_CHECK_FUNCS([bzero clock_gettime ftruncate gettimeofday inet_ntoa localtime_r memset mkdir socket strchr strerror])

AC_OUTPUT(Makefile 
          common/Makefile 
          src/Makefile)
