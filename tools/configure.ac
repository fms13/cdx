##
# \file configure.ac
#
# \date Oct 22, 2015
# \author Frank M. Schubert
#
# \brief Autotools configuration file for the CDX tools
#

AC_INIT([cdx-tools], [1.0])

# check if source folder is available (optional)
AC_CONFIG_SRCDIR([cdx-convert-continuous-to-discrete-src/cdx-convert-continuous-to-discrete.cpp])

# check for C++ preprocessor and compiler
AC_PROG_CXXCPP
AC_PROG_CXX

# automake initialization
AM_INIT_AUTOMAKE([1.9 tar-pax -Wall -Werror])

# use the C++ compiler for the following checks (optional)
AC_LANG([C++])

# to make M_PI available:
AC_DEFINE(_USE_MATH_DEFINES)

# check for Boost:
AC_CONFIG_MACRO_DIR([m4])
AX_BOOST_BASE([1.48], , [AC_MSG_ERROR([Boost not found])])
AX_BOOST_PROGRAM_OPTIONS

# Check for libraries
AC_CHECK_LIB([armadillo], [main], , [AC_MSG_ERROR([cannot find armadillo library])])
AC_CHECK_LIB([hdf5], [main], , [AC_MSG_ERROR([cannot find hdf5 library])])
AC_CHECK_LIB([hdf5_cpp], [main], , [AC_MSG_ERROR([cannot find hdf5_cpp library])])
AC_CHECK_LIB([cdx], [main], , [AC_MSG_ERROR([cannot find cdx library])])

# check for typedefs, structures and compiler characteristics (optional)
AC_TYPE_SIZE_T

# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
#AC_SUBST([AM_CXXFLAGS])
#AC_SUBST([AM_LDFLAGS])
#AC_SUBST([LIBS])

# files to generate via autotools (prepare .am source files)
AC_CONFIG_FILES([Makefile])

# generates the Makefiles etc. for the build
AC_OUTPUT
