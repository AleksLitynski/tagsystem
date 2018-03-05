.PHONY: clean all test src

ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

all: build

build: src test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

test:
	-$(MAKE) -C test all

src:
	-$(MAKE) -C src all
