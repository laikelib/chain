

DIRS = safeMiddle rpcServer core sos miner

all:
	for dir in ${DIRS}; do make -C $$dir -j4; echo ; done;


rebuild:
	for dir in ${DIRS}; do make -C $$dir rebuild; echo ; done;

clean:
	for dir in ${DIRS}; do make -C $$dir clean; echo ; done;


