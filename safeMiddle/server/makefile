##===================================================================##
## File Name: makefile
## Create Author: Tom Hui
## Create Date: Sun Oct 15 1850 2017
## Description:
##		
##===================================================================##

include ../../inc.mk

TARGET :=
TARGET_ME = ../${BIN_PATH}/server
ifneq "$(OS_NAME)" "Linux"
TARGET += ${TARGET_ME}.exe
else 
TARGET += ${TARGET_ME}
endif

SRC = $(wildcard *.cpp)
COMM_SRC = $(wildcard ../common/*.cpp)
SRC += ${COMM_SRC}

OBJS = ${patsubst %.cpp, %.o, ${SRC}}


CXX_INCLUDE += -I../../../huibase/src
LINK_CXX_LIBS += -L../../../huibase/lib

CXX_CFLAGS += -fstack-protector-all
LINK_CXX_CFLAGS += -fstack-protector-all 

ifeq "$(OS_NAME)" "Darwin"
LINK_CC_LIBS += -liconv
endif

LINK_CXX_LIBS += -ldl

TTYPE = EFILE

all: def

def: ${TARGET}

test_me:
	@echo ${OBJS}


include ../../route.mk

