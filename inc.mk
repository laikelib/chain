##===================================================================##
## File Name: inc.mk
## Create Author: Tom Hui
## Create Date: Tue Sep 12 1359 2015
## Description:
##		Makefile header. This include some declaration for using in 
##	makefile. I create this file for reduce duplication.
## DONOT MODIFY THIS FILE UNTIL YOU UNDERSTAND THE DETAIL.
##===================================================================##

CC = g++
AR_CMD = ar cr

CFLAGS = -g

BIN_PATH = ../bin
LIB_PATH = ../lib

CXX_CFLAGS = -O2 -g -Wall -Wextra -Werror -Wno-unknown-pragmas -fstack-protector-strong -fstack-protector -fPIC -std=c++11
CXX_DEFINES :=
CXX_INCLUDE = -I../ -I../include -I../src -I../

CC_CFLAGS :=
CC_DEFINES :=
CC_INCLUDE :=

LINK_CXX_CFLAGS :=
LINK_CXX_LIBS := -L../lib -lhuibase

LINK_CC_CFLAGS :=
LINK_CC_LIBS :=


SO_CXX_CFLAGS = -shared -fPIC
AR_CC_CFLAGS :=

.PHONY: all clean install test type_test

OS_NAME = $(shell uname)


CXX_INCLUDE += -I/data/laikelib/chain/deps/include \
	-I/data/laikelib/chain/deps/glog/src
LINK_CXX_LIBS += /data/laikelib/chain/deps/lib/libconfig++.a \
	/data/laikelib/chain/deps/glog/.libs/libglog.a


TTYPE = AFILE

TARGET :=


