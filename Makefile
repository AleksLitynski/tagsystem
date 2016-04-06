
SSLDIR = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb


rebuild: clean build
all: init rebuild
init:

build: lmdb openssl tagsystem

tagsystem:
	mkdir -p bin/
	gcc -std=c11 -o bin/tagsystem \
	    -I$(SSLDIR)/include/ \
	    $(wildcard *.h) $(wildcard *.c) \
	    $(MDBDIR)/liblmdb.a \
	    $(SSLDIR)/libcrypto.a

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
