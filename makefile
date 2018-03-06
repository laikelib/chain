

DIRS = safeMiddle core sos miner http rpcServer

all:
	for dir in ${DIRS}; do make -C $$dir -j8; echo ; done;


rebuild:
	for dir in ${DIRS}; do make -C $$dir rebuild; echo ; done;

clean:
	for dir in ${DIRS}; do make -C $$dir clean; echo ; done;


