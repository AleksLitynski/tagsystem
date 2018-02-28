.PHONY: clean all test src lmdb

all: build

build: lmdb src test

clean:
	rm -rf testdata
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(MAKE) -C lib/lmdb/libraries/liblmdb clean
	
test:
	$(MAKE) -C test all

src:
	$(MAKE) -C src all

lmdb:
	$(MAKE) -C lib/lmdb/libraries/liblmdb

