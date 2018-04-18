.PHONY: clean all run
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

SOURCE = $(shell echo *.c)
HEADER = $(shell echo *.h)
HEADER += $(shell echo ../lib/sds/*.h)
HEADER += $(shell echo ../lib/hash/*.h)
HEADER += $(shell echo ../lib/lmdb/libraries/liblmdb/*.h)
HEADER += ../lib/fs.fs.h
HEADER += $(shell echo ../src/*.h)
OBJECT = $(SOURCE:.c=.o)
TARGET = tsys

CC = gcc
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

LINKED = ../src/libts.a
ifeq ($(detected_os),Windows)
	LINKED += -lntdll
	LDLIBS = -lntdll 
	SOLIBS = -lntdll
endif
ifeq ($(detected_os),Linux)
	LINKED += -lpthread
endif

pwd: $(pwd)

all: $(TARGET)

clean:
	rm -f $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(LINKED)


run: $(TARGET)
	./tsys
