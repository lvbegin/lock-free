AUTOMAKE_OPTIONS = subdir-objects


TESTS=lock_free_check
check_PROGRAMS=lock_free_check
lock_free_check_SOURCES=test/test.cpp include/stack.h include/list.h
lock_free_check_CPPFLAGS=-I$(top_srcdir)/include
AM_LD_FLAGS=-lpthread
include_HEADERS=include/stack.h
