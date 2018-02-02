##===================================================================##
## File Name: makefile
##
## Create Author: Tom Hui
##
## Create Date: Thu Feb 1 1755 2018
##
## Description:
##		
##===================================================================##

include ../../inc.mk

TARGET = ../${LIB_PATH}/libclient.a

SRC = $(wildcard *.cpp)
COMM_SRC = $(wildcard ../../safeMiddle/common/*.cpp)
SRC += ${COMM_SRC}

OBJS = ${patsubst %.cpp, %.o, ${SRC}}


CXX_INCLUDE += -I../../../huibase/src
CXX_INCLUDE += -I../../core
CXX_INCLUDE += -I../../deps/jsoncpp/include
LINK_CXX_LIBS += -L../../../huibase/lib
LINK_CXX_LIBS += -lhuibase -lcore
LINK_CXX_LIBS += ../../deps/jsoncpp/build/src/lib_json/libjsoncpp.a
LINK_CXX_LIBS += -L../../deps/glog/.libs/ -lglog -lpthread
LINK_CXX_LIBS += -L../../deps/db48/build_unix -ldb_cxx -lpthread
LINK_CXX_LIBS += ../../deps/leveldb/libleveldb.a -lpthread
LINK_CXX_LIBS += -L../../deps/openssl -lssl -lcrypto -lpthread

CXX_CFLAGS += -fstack-protector-all
LINK_CXX_CFLAGS += -fstack-protector-all 

ifeq "$(OS_NAME)" "Darwin"
LINK_CC_LIBS += -liconv
endif

LINK_CXX_LIBS += -ldl

all: def

def: ${TARGET}

test_me:
	@echo ${OBJS}


include ../../route.mk

