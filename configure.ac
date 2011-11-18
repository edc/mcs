#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(MCS, 0.1.0, ycao@cs.ucr.edu)
AC_CONFIG_SRCDIR([vmax.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC


AC_ARG_WITH(libgc-include, AC_HELP_STRING([--with-libgc-include],
	[specify path to a directory containing libgc header files]),
	[],
	[with_libgc_include="default"])
AC_ARG_WITH(libgc-lib, AC_HELP_STRING([--with-libgc-lib],
	[specify path to a directory containing libgc libs (static and dynamic)]),
	[],
	[with_libgc_lib="default"])

if test "x$with_libgc_include" == "xyes" || test "x$with_libgc_include" == "xdefault"
then
	AC_CHECK_HEADERS([gc/gc.h], , [pass="no"])
else
	AC_SUBST(CONF_CFLAGS, [-I$with_libgc_include])
	AC_SUBST(LIBGCINC, [$with_libgc_include])
	AC_CHECK_HEADERS([$with_libgc_include/gc.h], [], [pass="no-reset"])
fi

if test "x$pass" == "xno"
then
	AC_MSG_ERROR([cannot find header file gc.h. Consider setting --with-libgc-include=PATH_TO_LIBGC_INCLUDE_DIR])
else 
	if test "x$pass" == "xno-reset"
	then
		AC_MSG_ERROR([cannot find header file gc.h. Consider resetting --with-libgc-include=PATH_TO_LIBGC_INCLUDE_DIR])
	fi
fi

if test "x$with_libgc_lib" == "xyes" || test "x$with_libgc_lib" == "xdefault"
then
	AC_CHECK_LIB([gc], [GC_malloc], , [pass="no"])
else
	AC_SUBST(LDFLAGS, [-L$with_libgc_lib])
	AC_SUBST(LIBGCDIR, [$with_libgc_lib])
	AC_CHECK_LIB([gc -L$with_libgc_lib], [GC_malloc], , [pass="no-reset"])
fi
if test "x$pass" == "xno"
then
	AC_MSG_ERROR([cannot link libgc. Consider setting --with-libgc-lib=PATH_TO_LIBGC_INCLUDE_DIR])
else
	if test "x$pass" == "xno-reset"
	then
		AC_MSG_ERROR([cannot link libgc. Consider resetting --with-libgc-lib=PATH_TO_LIBGC_INCLUDE_DIR])
	fi
fi

# Check whether to make python and R
BUILD_PY=''
BUILD_R=''
AC_ARG_ENABLE(python, AC_HELP_STRING([--enable-python], [make python module (default is disabled)]),BUILD_PY=yes,)
AC_ARG_ENABLE(R, AC_HELP_STRING([--enable-R], [make R package (default is disabled)]),BUILD_R=yes,)
if test "x$BUILD_PY" = "xyes"
then
	AC_SUBST(WITHPYTHON, python)
fi
if test "x$BUILD_R" = "xyes"
then
	AC_SUBST(WITHR, R)
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

AC_CONFIG_FILES([Makefile
		python/setup.py
		tests/Makefile
		R/mcs/src/Makevars])
AC_OUTPUT

AC_MSG_NOTICE([You may now invoke `make'])
