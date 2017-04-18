lib_LTLIBRARIES = libcdx.la

AUTOMAKE_OPTIONS = subdir-objects

libcdx_la_SOURCES = cdx/File.cpp \
	cdx/WriteFile.cpp \
	cdx/WriteContinuousDelayFile.cpp \
	cdx/WriteDiscreteDelayFile.cpp \
	cdx/ReadFile.cpp \
	cdx/ReadContinuousDelayFile.cpp

#	cdx/ReadDiscreteDelayFile.cpp
#	cdx/ReadDiscreteDelayFile.h

libcdx_la_LIBADD = -lhdf5 -lhdf5_cpp

# not using pkginclude_HEADERS here because that puts the headers into /usr/include/libcdx but we want
# them to be in /usr/include/cdx: 
otherincludedir = $(includedir)/cdx
otherinclude_HEADERS = cdx/File.h \
	cdx/WriteFile.h \
	cdx/WriteContinuousDelayFile.h \
	cdx/WriteDiscreteDelayFile.h \
	cdx/ReadFile.h \
	cdx/ReadContinuousDelayFile.h

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -std=c++11 -Wall -O3
