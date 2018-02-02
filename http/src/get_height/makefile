

include ../../../inc.mk

TARGET = ../${BIN_PATH}/get_height.cgi

SRC = $(wildcard *.cpp)

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I ../../../../huibase/deps/mysql++/lib \
	-I/usr/include/mysql \
	-I../../../../huibase/src \
	-I../../../../huibase/db \
	-I../../cgicomm \
	-I../../../core


CXX_CFLAGS = -fPIC -O2 -g -Wall -std=c++11
LINK_CXX_LIBS += -L../../lib \
	-L../../../../huibase/lib \
	-L../../../../huibase/deps/mysql++ \
	-L../../../deps/libconfig/lib/.libs -lconfig++ \
	-lcgi -lhuidb -lhuibase -lcgicc

TTYPE = EFILE

all: def

def: ${TARGET}


include ../../../route.mk
