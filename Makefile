
MDBDIR = lib/lmdb/libraries/liblmdb
KLBDIR = lib/klib
LDFLAGS="-Wl,-R -Wl,/home/ajl/projects/tagsystem/bin"

all: clean build

build: test-main 

clean: 
	rm -rf bin/
	cd $(MDBDIR) && $(MAKE) clean

# errors are ignore because klib is a stinker
lib-ts: lib-lmdb
	mkdir -p bin/
	cd bin && gcc -std=c11 -D_GNU_SOURCE -Werror -Wfatal-errors \
	    -Wno-error=discarded-qualifiers \
	    -Wno-discarded-qualifiers \
	    -I../$(MDBDIR)/ \
	    -I../$(KLBDIR)/ \
	    -I../src/ \
	    -L../bin/ \
	    -c 	../src/ts.c \
		../src/tsdoc.c \
		../src/tsenv.c \
		../src/tsiter.h \
		../src/tsnode.c \
		../src/tssearch.c \
		../src/tstag.c \
		../src/tsutil.c \
		../src/tswalk.c 
	cd bin && ar rvs libts.a \
		../src/ts.c \
		../src/tsdoc.c \
		../src/tsenv.c \
		../src/tsiter.h \
		../src/tsnode.c \
		../src/tssearch.c \
		../src/tstag.c \
		../src/tsutil.c \
		../src/tswalk.c \
		../bin/liblmdb.a

lib-lmdb: 
	mkdir -p bin/
	cd $(MDBDIR) && $(MAKE)
	cp $(MDBDIR)/liblmdb.a bin/liblmdb.a

test-main: lib-ts
	mkdir -p bin/
	gcc -std=c11 -Werror -Wfatal-errors \
	    -Isrc/ \
	    -I$(MDBDIR)/ \
	    -Lbin/ \
	    -o bin/test \
	    test/test.c \
	    bin/libts.a

test-redir:
	gcc -std=c11 -Werror -Wfatal-errors test/redir.c -o bin/redir
	bin/redir

test-addremove: clean
	mkdir -p bin/
	export LDFLAGS="-R ./" && gcc -std=c11 -Werror -Wfatal-errors \
	    -I$(MDBDIR)/ \
	    -L$(MDBDIR)/ \
	    -o bin/addremovetest \
	    test/addremove.c \
	    -llmdb 
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR) && ./addremovetest

