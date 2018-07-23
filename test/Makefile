.PHONY: clean all run
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

SOURCE = $(shell echo *.c) $(shell ls ../lib/cmocka/src/cmocka.c)
OBJECT = $(SOURCE:.c=.o)
SOTARGET = testshared
TARGET = test

CC = gcc
CFLAGS = $(CMOCKADEFS)
CFLAGS += -I ../lib/cmocka/include
CFLAGS += -I ../lib/fs.c
CFLAGS += -I ../lib/lmdb/libraries/liblmdb
CFLAGS += -I ../lib/sds
CFLAGS += -I ../lib/hash
CFLAGS += -I ../src
CFLAGS += -g 
CFLAGS += -Wfatal-errors
CFLAGS += -std=c11
CFLAGS += -D_GNU_SOURCE
CFLAGS += -Werror
CFLAGS += -Wno-error=discarded-qualifiers
CFLAGS += -Wno-discarded-qualifiers
CFLAGS += -Wno-format-extra-args
 

LINKED = 
SHAREDLIBPATH = ../src
ifeq ($(detected_os),Windows)
	CFLAGS += -imacros ./cmocka.win.def.h
	LINKED += -lntdll
	LDLIBS = -lntdll 
	SOLIBS = -lntdll
endif
ifeq ($(detected_os),Linux)
	CFLAGS += -imacros ./cmocka.unix.def.h
	LINKED += -lpthread
endif

pwd: $(pwd)

all: $(TARGET) $(SOTARGET)

clean:
	rm -f $(OBJECT) $(TARGET) $(SOTARGET)
	rm -rf test_db

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) ../src/libts.a $(LINKED)

$(SOTARGET): $(OBJECT)
	gcc -L$(SHAREDLIBPATH) $(CFLAGS) -o $@ $(OBJECT) -lts $(LINKED)

run: $(TARGET)
	rm -rf test_db
	./test

runshared: $(SOTARGET)
	rm -rf test_db
	LD_LIBRARY_PATH=$(SHAREDLIBPATH) ./testshared