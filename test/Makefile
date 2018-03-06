.PHONY: clean all run
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

SOURCE = $(shell echo *.c) $(shell ls ../lib/cmocka/src/cmocka.c)
HEADER = $(shell echo *.h)
HEADER += $(shell ls ../lib/cmocka/include/cmocka.h)
HEADER += $(shell echo ../lib/sds/*.h)
HEADER += $(shell echo ../lib/lmdb/libraries/liblmdb/*.h)
HEADER += ../lib/fs.fs.h
HEADER += $(shell echo ../src/*.h)
OBJECT = $(SOURCE:.c=.o)
TARGET = test

CC = gcc
CFLAGS = $(CMOCKADEFS)
CFLAGS += -imacros ./cmocka.def.h
CFLAGS += -I ../lib/cmocka/include
CFLAGS += -I ../lib/fs.c
CFLAGS += -I ../lib/lmdb/libraries/liblmdb
CFLAGS += -I ../lib/sds
CFLAGS += -I ../src
CFLAGS += -g -Wfatal-errors

LINKED = ../src/libts.a
ifeq ($(detected_os),Windows)
	LINKED += -lntdll
endif
ifeq ($(detected_os),Linux)
	LINKED += -lpthread
endif


all: $(TARGET)

clean:
	rm -f $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(LINKED)


run: $(TARGET)
	./test
