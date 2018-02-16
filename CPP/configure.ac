AC_INIT([libcdx], [1.3])
AM_INIT_AUTOMAKE([1.9 -Wall -Werror])

AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT

AC_CHECK_LIB([hdf5], [main], , [AC_MSG_ERROR([cannot find hdf5 library])])
AC_CHECK_LIB([hdf5_cpp], [main], , [AC_MSG_ERROR([cannot find hdf5_cpp library])])

AC_PROG_CXX

AC_OUTPUT([Makefile])
