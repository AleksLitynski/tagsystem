
SSLDIR = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb


ts: openssl lmdb
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors -shared -fPIC \
	    -I$(SSLDIR)/include/ \
	    -I$(MDBDIR)/ \
	    -Isrc/ \
	    -o bin/libts.so \
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

buildtest: 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -Isrc/ \
	    -o bin/test \
	    test/test.c

runtest: buildtest
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(MDBDIR):$(SSLDIR):bin/ && bin/test
