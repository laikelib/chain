##===================================================================##
## File Name: makefile
## Create Author: Tom Hui
## Create Date: Fri Oct 20 1339 2017
## Description:
##		
##===================================================================##

include ../../inc.mk

TARGET = ../${LIB_PATH}/libmiddleso.a

SRC = $(wildcard *.cpp)

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I../../../huibase/src -I../../deps/glog/src
LINK_CXX_LIBS += -L../../../huibase/lib

ifeq "$(OS_TYPE)" "Darwin"
LINK_CC_LIBS += -liconv
endif

all: def

def: ${TARGET}


include ../../route.mk

