
LMDBDIR = lmdb/libraries/liblmdb/
NODEGYP = node_modules/node-gyp/bin/node-gyp.js

rebuild: clean build
all: init clean build

init:
	npm install node-gyp

XCFLAGS=-DANDROID
export XCFLAGS
build:
	$(MAKE) -C $(LMDBDIR) 
	$(NODEGYP) rebuild

clean:
	$(MAKE) -C $(LMDBDIR) clean
	$(NODEGYP) clean


# sudo.sh rm -fr build/
# node-gyp clean
# node-gyp configure
# cd build/
# sudo.sh make CC=gcc
# sudo.sh chown -R u0_a84:u0_a84 Release/
