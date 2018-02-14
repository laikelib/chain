

DIRS = get_height height_block new_account inner_tran account_info transfer

all:
	for dir in ${DIRS}; do make -C $$dir -j4; echo ; done;

clean:
	for dir in ${DIRS}; do make -C $$dir clean; echo ; done;
