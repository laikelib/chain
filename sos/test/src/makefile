##===================================================================##
## File Name: makefile
## Create Author: Tom Hui
## Create Date: Fri Oct 20 1339 2017
## Description:
##		
##===================================================================##

include ../../../inc.mk


TARGET = ${LIB_PATH}/libsctest.so

SRC = $(wildcard *.cpp)

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I../../../../huibase/src
CXX_INCLUDE += -I../../../safeMiddle/middleSo

LINK_CXX_LIBS += -L../../../../huibase/lib
LINK_CXX_LIBS += -L../../../lib -lmiddleso -lhuibase

ifeq "$(OS_TYPE)" "Darwin"
LINK_CC_LIBS += -liconv
endif

TTYPE = SFILE

all: def

def: ${TARGET}


include ../../../route.mk

