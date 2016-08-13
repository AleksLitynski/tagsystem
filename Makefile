
MDBDIR = lib/lmdb/libraries/liblmdb
KLBDIR = lib/klib
MURDIR = lib/murmurhash.c

main: cleanbin ts runtest

# errors are ignore because klib is a stinker
ts: 
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

all: ts lmdb

lmdb: 
	cd $(MDBDIR) && $(MAKE) XCFLAGS=-DANDROID
clean: cleanbin
	cd $(MDBDIR) && $(MAKE) clean

cleanbin: 
	rm -rf bin/

buildtest: 
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

runtest: buildtest lmdb
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR):./ && ./test

iter:
	gcc -std=c11 -Werror -Wfatal-errors utils/iter.c -o bin/iter
	bin/iter


redir:
	gcc -std=c11 -Werror -Wfatal-errors test/redir.c -o bin/redir
	bin/redir

addremove: cleanbin 
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -I$(MDBDIR)/ \
	    -L$(MDBDIR)/ \
	    -o bin/addremovetest \
	    test/addremove.c \
	    -llmdb 
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR) && ./addremovetest

