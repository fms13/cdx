lib_LTLIBRARIES = libcdx.la

AUTOMAKE_OPTIONS = subdir-objects

libcdx_la_SOURCES = src/File.cpp \
	src/WriteFile.cpp \
	src/WriteContinuousDelayFile.cpp \
	src/WriteDiscreteDelayFile.cpp \
	src/ReadFile.cpp \
	src/ReadContinuousDelayFile.cpp

#	src/ReadDiscreteDelayFile.cpp
#	src/ReadDiscreteDelayFile.h

libcdx_la_LIBADD = -lhdf5 -lhdf5_cpp

# not using pkginclude_HEADERS here because that puts the headers into /usr/include/libcdx but we want
# them to be in /usr/include/cdx: 
otherincludedir = $(includedir)/cdx
otherinclude_HEADERS = src/File.h \
	src/WriteFile.h \
	src/WriteContinuousDelayFile.h \
	src/WriteDiscreteDelayFile.h \
	src/ReadFile.h \
	src/ReadContinuousDelayFile.h

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -std=c++11 -Wall -O3
