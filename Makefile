
SSLDIR  = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb

XCFLAGS=-DANDROID
export XCFLAGS

rebuild: clean build
all: init rebuild
init:

build: lmdb openssl
	mkdir bin/
	gcc -std=c11 -o bin/tagsystem \
	    -I$(SSLDIR)/include/ \
	    tagsystem.h tagsystem.c \
	    $(MDBDIR)/liblmdb.a \
	    $(SSLDIR)/libcrypto.a

lmdb: 
	$(MAKE) -C $(MDBDIR)
openssl:
	cd $(SSLDIR) && perl Configure android
	cd $(SSLDIR) && $(MAKE) -C $(SSLDIR)
clean:
	rm -rf bin/
	$(MAKE) -C $(MDBDIR) clean
	cd $(SSLDIR) && $(MAKE) -C $(SSLDIR) clean
test: 
	./bin/tagsystem

# openssltest:
	# gcc -std=c11 -o bin/ssltest \
	#     -Ilib/openssl/include/ \
	#     openssl-test.c \
	#     lib/openssl/libcrypto.a
