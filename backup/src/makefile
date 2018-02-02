##===================================================================##
## File Name: makefile
## Create Author: Tom Hui
## Create Date: Fri Oct 20 1339 2017
## Description:
##		
##===================================================================##

include ../../../inc.mk


TARGET = ${LIB_PATH}/libscmain.so

SRC = $(wildcard *.cpp)

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I../../../../huibase/src -I../../../core
CXX_INCLUDE += -I../../../safeMiddle/middleSo
CXX_INCLUDE += -I../../../deps/leveldb/include
CXX_INCLUDE += -I../../../deps/leveldb/include

LINK_CXX_LIBS += -L../../../../huibase/lib
LINK_CXX_LIBS += -L../../../lib -lmiddleso -lhuibase -lcore
LINK_CXX_LIBS += -L../../../deps/glog/.libs/ -lglog
LINK_CXX_LIBS += -L../../../deps/db48build_unix -ldb_cxx
LINK_CXX_LIBS += ../../../deps/leveldb/libleveldb.a
LINK_CXX_LIBS += -L../../../deps/openssl -lssl -lcrypto


ifeq "$(OS_TYPE)" "Darwin"
LINK_CC_LIBS += -liconv
endif

TTYPE = SFILE

all: def

def: ${TARGET}


include ../../../route.mk

