##===================================================================##
## File Name: makefile
## Create Author: Tom Hui
## Create Date: Fri Oct 20 1339 2017
## Description:
##		
##===================================================================##

include ../inc.mk

TARGET = ${LIB_PATH}/libcore.a

SRC = $(wildcard *.cpp)

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I../../huibase/src -I../deps/glog/src -I../deps/openssl/include \
	-I../deps/db48/build_unix -I../deps/leveldb/include -I../deps/leveldb/helpers -I../deps/jsoncpp/include
LINK_CXX_LIBS += -L../../huibase/lib -lglog -ldb_cxx -lleveldb

ifeq "$(OS_TYPE)" "Darwin"
LINK_CC_LIBS += -liconv
endif

all: def

def: ${TARGET}


include ../route.mk

