
SSLDIR = lib/openssl
MDBDIR = lib/lmdb/libraries/liblmdb
KLBDIR = lib/klib

main: cleanbin ts runtest

# errors are ignore because klib is a stinker
ts: 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors -shared -fPIC \
	    -Wno-error=discarded-qualifiers \
	    -Wno-discarded-qualifiers \
	    -I$(SSLDIR)/include/ \
	    -I$(MDBDIR)/ \
	    -I$(KLBDIR)/ \
	    -Isrc/ \
	    -L$(SSLDIR)/ \
	    -L$(MDBDIR)/ \
	    -o bin/libts.so \
	    src/ts.c \
	    src/tsdoc.c \
	    src/tsenv.c \
	    src/tsnode.c \
	    src/tssearch.c \
	    src/tstag.c \
	    src/tsutil.c \
	    src/tswalk.c \
	    -llmdb \
	    -lcrypto

all: ts lmdb openssl

lmdb: 
	cd $(MDBDIR) && $(MAKE) XCFLAGS=-DANDROID
openssl:
	cd $(SSLDIR) && perl Configure android
	cd $(SSLDIR) && $(MAKE) -i SHELL=/system/bin/sh
clean: cleanbin
	cd $(MDBDIR) && $(MAKE) clean
	cd $(SSLDIR) && $(MAKE) clean SHELL=/system/bin/sh

cleanbin: 
	rm -rf bin/

buildtest: 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -Isrc/ \
	    -I$(MDBDIR)/ \
	    -I$(SSLDIR)/include/ \
	    -Lbin/ \
	    -L$(MDBDIR)/ \
	    -L$(SSLDIR)/ \
	    -o bin/test \
	    test/test.c \
	    -llmdb \
	    -lcrypto \
	    -lts

runtest: buildtest lmdb
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR):../$(SSLDIR):./ && ./test

iter:
	gcc -std=c11 -Werror -Wfatal-errors utils/iter.c -o bin/iter
	bin/iter


redir:
	gcc -std=c11 -Werror -Wfatal-errors test/redir.c -o bin/redir
	bin/redir
