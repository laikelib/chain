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
CXX_INCLUDE += -I../../../../huibase/crypto
CXX_INCLUDE += -I../../../safeMiddle/middleSo
CXX_INCLUDE += -I../../../deps/leveldb/include
CXX_INCLUDE += -I../../../deps/openssl/include
CXX_INCLUDE += -I../../../deps/jsoncpp/include

LINK_CXX_LIBS += -L../../../../huibase/lib
LINK_CXX_LIBS += -L../../../lib -lmiddleso -lhuibase -lcore -lpthread
LINK_CXX_LIBS += ../../../deps/jsoncpp/build/src/lib_json/libjsoncpp.a
LINK_CXX_LIBS += -L../../../deps/glog/.libs/ -lglog -lpthread
LINK_CXX_LIBS += -L../../../deps/db48/build_unix -lpthread
LINK_CXX_LIBS += ../../../deps/leveldb/libleveldb.a -lpthread
LINK_CXX_LIBS += ../../../deps/openssl/libssl.a ../../../deps/openssl/libcrypto.a -lpthread
LINK_CXX_LIBS += -L../../../../huibase/lib
LINK_CXX_LIBS += -lhuibase -lhuicrypto


ifeq "$(OS_TYPE)" "Darwin"
LINK_CC_LIBS += -liconv
endif

TTYPE = SFILE

all: def

def: ${TARGET}


include ../../../route.mk

