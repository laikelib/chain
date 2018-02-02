

include ../../inc.mk

TARGET = ${LIB_PATH}/libcgi.a

SRC = $(wildcard *.cpp)
COMM_SRC = $(wildcard ../../safeMiddle/common/*.cpp)
SRC += ${COMM_SRC}

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I../../../huibase/deps/mysql++/lib \
	-I/usr/include/mysql \
	-I../../../huibase/src \
	-I../../deps/libconfig/lib \
	-I../../../huibase/db \
	-I../../core

CXX_CFLAGS = -fPIC -O2 -g -Wall -std=c++11

all: def

def: ${TARGET}


include ../../route.mk






