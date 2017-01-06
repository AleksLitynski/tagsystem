
MDBDIR = lib/lmdb/libraries/liblmdb
KLBDIR = lib/klib

all: build

build: test-main

clean:
	rm -rf bin/
	cd $(MDBDIR) && $(MAKE) clean

# errors are ignore because klib is a stinker
lib-ts: lib-lmdb
	mkdir -p bin/
	cd bin/ \
	&& gcc -g -std=c11 -D_GNU_SOURCE -Werror -Wfatal-errors \
		-Wno-error=discarded-qualifiers -Wno-discarded-qualifiers \
		-I../$(MDBDIR)/ \
		-I../$(KLBDIR)/ \
		-I../src/ \
		-c 	../src/*.c \
		-lpthread \
	&& ar rvs libts.a \
		*.o \
		liblmdb.a

lib-lmdb:
	mkdir -p bin/
	cd $(MDBDIR) && $(MAKE)
	cp $(MDBDIR)/liblmdb.a bin/liblmdb.a

test-main: lib-ts
	mkdir -p bin/
	gcc -g -o2 -std=c11 -Werror -Wfatal-errors \
	    -Isrc/ \
	    -I$(MDBDIR)/ \
	    -Lbin/ \
	    -o bin/test \
	    test/test.c \
	    bin/libts.a \
	    bin/liblmdb.a \
	    -lpthread


test-redir:
	gcc -g -std=c11 -Werror -Wfatal-errors test/redir.c -o bin/redir
	bin/redir

test-addremove: clean
	mkdir -p bin/
	export LDFLAGS="-R ./" && gcc -g -std=c11 -Werror -Wfatal-errors \
	    -I$(MDBDIR)/ \
	    -L$(MDBDIR)/ \
	    -o bin/addremovetest \
	    test/addremove.c \
	    -llmdb
	cd bin/ && export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../$(MDBDIR) && ./addremovetest
