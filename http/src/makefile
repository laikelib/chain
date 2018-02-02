

DIRS = get_height

all:
	for dir in ${DIRS}; do make -C $$dir -j4; echo ; done;

clean:
	for dir in ${DIRS}; do make -c $$dir clean; echo ; done;
