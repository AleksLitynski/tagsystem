
MDBDIR = ../lib/lmdb/libraries/liblmdb


doit:
	rm -rf lmdbtestdir
	mkdir lmdbtestdir
	gcc -std=c11 -Werror -Wfatal-errors \
	    -Wno-error=discarded-qualifiers \
	    -Wno-discarded-qualifiers \
	    -I$(MDBDIR)/ \
	    -L$(MDBDIR)/ \
	    -o lmdbtest \
	    lmdbtest.c \
	    -llmdb
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(MDBDIR):./ && ./lmdbtest
