
LMDBDIR=$(abspath $(CURDIR)/../lmdb/libraries/liblmdb/)

test: FORCE 
	cp $(LMDBDIR)/liblmdb.a ./liblmdb.a	
	cp $(LMDBDIR)/lmdb.h ./lmdb.h	
	gcc -std=c11 -o test \
	    test.c \
	    liblmdb.a 
	./test

FORCE:


clean:
	rm -f test
	rm -f liblmdb.a
	rm -f lmdb.h 
