.PHONY: clean all test src cli

ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

all: build

build: src test cli


clean:
	-$(MAKE) -C src clean
	-$(MAKE) -C test clean
	-$(MAKE) -C cli clean

test:
	-$(MAKE) -C test all

src:
	-$(MAKE) -C src all

cli:
	-$(MAKE) -C cli all

runtest:
	$(MAKE) -C test run
	$(MAKE) -C test runshared
