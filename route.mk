##===================================================================##
## File Name: route.mk
## Create Author: Tom Hui
## Create Date: Tue Sep 12 1359 2015
## Description:
##		Makefile header. This include some declaration for using in 
##	makefile. I create this file for reduce duplication.
## DONOT MODIFY THIS FILE UNTIL YOU UNDERSTAND THE DETAIL.
##===================================================================##

type_test:
	@if [ "$(TTYPE)" = "AFILE" ]; then echo "\033[33m=====>We Are Compiling static Library File: ${TARGET}\033[0m";  fi
	@if [ "$(TTYPE)" = "EFILE" ]; then echo "\033[33m=====>We Are Compiling Execute File: ${TARGET}\033[0m"; fi
	@if [ "$(TTYPE)" = "SFILE" ]; then echo "\033[33m=====>We Are Compiling Share library File: ${TARGET}\033[0m"; fi

clean:
	@echo "\033[31m=================Cleanning=================\033[0m"
	rm -f ${OBJS}; \
	rm -f ${TARGET};
	@echo "\033[31m*****************Cleanning*****************\033[0m"



test:
	@echo "\033[31m=================test=================\033[0m"
	@echo ${OBJS}; \
	@echo ${TTYPE};
	@echo "\033[31m*****************test*****************\033[0m"


%.o: %.cpp
	@echo "\033[31m===COMPILING OBJECT===: $@\033[0m"
	${CC} ${CXX_DEFINES} ${CXX_INCLUDE} ${CXX_CFLAGS} -o $@ -c $<

%.o: %.c
	@echo "\033[31m===COMPILING OBJECT===: $@\033[0m"
	${CC} ${CXX_DEFINES} ${CXX_INCLUDE} ${CXX_CFLAGS} -o $@ -c $<

GEN_CMD :=


ifeq "${TTYPE}" "AFILE"
GEN_CMD = ${AR_CMD} ${TARGET} ${OBJS}
endif

ifeq "${TTYPE}" "EFILE"
GEN_CMD = ${CC} ${LINK_CXX_CFLAGS} ${OBJS} -o ${TARGET} ${LINK_CXX_LIBS} 
endif 

ifeq "${TTYPE}" "SFILE"
GEN_CMD = ${CC} ${SO_CXX_CFLAGS} ${OBJS} -o ${TARGET} ${LINK_CXX_LIBS} 
endif


${TARGET}: type_test ${OBJS}
	@echo "\033[31m=================Linking Target: $@=================\033[0m"
	${GEN_CMD} 
	@echo "\033[32m****************************************************\033[0m"
	@echo "\033[32m******************MAKING COMPLETE******************\033[0m"
	@echo "\033[32m****************************************************\033[0m"

