.PHONY: clean all test src lmdb

ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

all: build

build: lmdb src test

clean:
	rm -rf testdata
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(MAKE) -C lib/lmdb/libraries/liblmdb clean

test:
	-$(MAKE) -C test all

src:
	-$(MAKE) -C src all

ifeq ($(detected_os),Windows)
	LMDBOVERIDES = LDLIBS='-lntdll' SOLIBS='-lntdll'
endif

lmdb:
	$(MAKE) -C lib/lmdb/libraries/liblmdb $(LMDBOVERIDES)
