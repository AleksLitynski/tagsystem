
SSLDIR = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb


rebuild: clean build
all: init rebuild
init:


ts: 
	mkdir -p bin/
	gcc \
	    -std=c11 -Werror -Wfatal-errors -shared -fPIC \
	    -o bin/libts.so \
	    -I$(SSLDIR)/include/ \
	    src/ts.c \
	    src/tsdoc.c \
	    src/tsenv.c \
	    src/tsnode.c \
	    src/tssearch.c \
	    src/tstag.c \
	    src/tsutil.c \
	    src/tswalk.c

lmdb: 
	cd $(MDBDIR) && $(MAKE) XCFLAGS=-DANDROID
openssl:
	cd $(SSLDIR) && perl Configure android
	cd $(SSLDIR) && $(MAKE) -i SHELL=/system/bin/sh
clean:
	rm -rf bin/
	cd $(MDBDIR) && $(MAKE) clean
	cd $(SSLDIR) && $(MAKE) clean SHELL=/system/bin/sh
test: ts
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -shared \
	    -Lbin/ \
	    -L$(MDBDIR) \
	    -L$(SSLDIR) \
	    -o bin/test \
	    test/test.c \
	    -lts \
	    -llmdb \
	    -lcrypto
