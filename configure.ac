#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lock-free], [1])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_INIT_AUTOMAKE
# Checks for libraries.
AC_CHECK_LIB(pthread, pthread_create)
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_LANG([C++])
#AX_CXX_COMPILE_STDCXX_11
CXXFLAGS='-Wall -std=gnu++0x'
# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
