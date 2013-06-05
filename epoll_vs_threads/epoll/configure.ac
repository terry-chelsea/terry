#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([epoll_test], [1.0], [fengyuatad@126.com])
AC_CONFIG_SRCDIR([src/Login_task.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(epoll_test , 1.0)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcrypto':
AC_CHECK_LIB([crypto], [main])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lprotobuf':
AC_CHECK_LIB([protobuf], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])
# FIXME: Replace `main' with a function in `-lzdb':
AC_CHECK_LIB([zdb], [main])
# FIXME: Replace `main' with a function in `-lzdbc':
AC_CHECK_LIB([zdbc], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MKTIME
AC_CHECK_FUNCS([bzero clock_gettime gettimeofday inet_ntoa localtime_r memset socket strchr strerror])

AC_OUTPUT([Makefile
           ThreadPool/Makefile
           common/Makefile
           database/Makefile])
