
MDBDIR = lib/lmdb/libraries/liblmdb
KLBDIR = lib/klib

all: lib-ts lib-lmdb test-main

clean: 
	rm -rf bin/
	cd $(MDBDIR) && $(MAKE) clean

# errors are ignore because klib is a stinker
lib-ts: 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors -shared -fPIC \
	    -Wno-error=discarded-qualifiers \
	    -Wno-discarded-qualifiers \
	    -I$(MDBDIR)/ \
	    -I$(KLBDIR)/ \
	    -I$(MURDIR)/ \
	    -Isrc/ \
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
	    -llmdb

lib-lmdb: 
	cd $(MDBDIR) && $(MAKE) XCFLAGS=-DANDROID

test-main:
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -Isrc/ \
	    -I$(MDBDIR)/ \
	    -Lbin/ \
	    -L$(MDBDIR)/ \
	    -o bin/test \
	    test/test.c \
	    -llmdb \
	    -lcrypto \
	    -lts

test-redir:
	gcc -std=c11 -Werror -Wfatal-errors test/redir.c -o bin/redir
	bin/redir

test-addremove: cleanbin 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -I$(MDBDIR)/ \
	    -L$(MDBDIR)/ \
	    -o bin/addremovetest \
	    test/addremove.c \
	    -llmdb 
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR) && ./addremovetest

