
SSLDIR = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb


rebuild: clean build
all: init rebuild
init:

build: lmdb openssl ts

ts:
	mkdir -p bin/
	gcc -std=c11 \
	    -Werror \
	    -Wfatal-errors \
	    -C -o bin/libts.o \
	    -I$(SSLDIR)/include/ \
	    $(MDBDIR)/liblmdb.a \
	    $(SSLDIR)/libcrypto.a \
	    src/ts.c \
	    src/tsdoc.c \
	    src/tsenv.c \
	    src/tsnode.c \
	    src/tssearch.c \
	    src/tstag.c \
	    src/tsutil.c \
	    src/tswalk.c 
	ar rcs bin/libts.a bin/libts.o

lmdb: 
	cd $(MDBDIR) && $(MAKE) XCFLAGS=-DANDROID
openssl:
	cd $(SSLDIR) && perl Configure android
	cd $(SSLDIR) && $(MAKE) -i SHELL=/system/bin/sh
clean:
	rm -rf bin/
	cd $(MDBDIR) && $(MAKE) clean
	cd $(SSLDIR) && $(MAKE) clean SHELL=/system/bin/sh
test: 
	./bin/tagsystem

# openssltest:
	# gcc -std=c11 -o bin/ssltest \
	#     -Ilib/openssl/include/ \
	#     openssl-test.c \
	#     lib/openssl/libcrypto.a
