
LMDBDIR = lib/lmdb/libraries/liblmdb/
XCFLAGS=-DANDROID
export XCFLAGS

rebuild: clean build
all: init rebuild

init:

build: lmdb
	gcc -std=c11 -o bin/tagsystem \
	    tagsystem.h tagsystem.c \
	    bin/liblmdb.a

lmdb: bin
	$(MAKE) -C $(LMDBDIR)
	cp $(LMDBDIR)/liblmdb.a bin/
bin: 
	mkdir bin/
clean:
	rm -rf bin/
	$(MAKE) -C $(LMDBDIR) clean
test: 
	./bin/tagsystem

openssltest:
	gcc -std=c11 -o bin/ssltest \
	    -Ilib/openssl/include/ \
	    openssl-test.c \
	    lib/openssl/libcrypto.a
