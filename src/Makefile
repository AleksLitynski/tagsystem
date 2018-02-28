.PHONY: clean all
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

SOURCE = $(shell echo *.c) $(shell echo ../lib/sds/*.c) $(shell echo ../lib/fs.c/*.c)
HEADER = $(shell echo *.h) $(shell echo ../lib/lmdb/libraries/liblmdb/*.h) $(shell echo ../lib/sds/*.h) $(shell echo ../lib/fs.c/*.c)
OBJECT = $(SOURCE:.c=.o)
TARGET = libts.a

LINKED  = ../lib/lmdb/libraries/liblmdb/liblmdb.a

ifeq ($(detected_os),Windows)
	CFLAGS = -g
	LDFLAGS = 
	LINKED += -lntdll
endif
ifeq ($(detected_os),Linux)
	CFLAGS = -fPIC -g
endif
CC = gcc


# For shared library linking - 
# LDFLAGS = -shared
# LINKED  = ../lib/lmdb/libraries/liblmdb/liblmdb.so

CFLAGS += -std=c11 
CFLAGS += -D_GNU_SOURCE 
CFLAGS += -Werror 
CLFAGS += -Wfatal-errors 
CFLAGS += -Wno-error=discarded-qualifiers 
CLFAGS += -Wno-discarded-qualifiers

all: $(TARGET)

clean:
	rm -f $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $(SOURCE) -o $@ $(LDFLAGS) $(LINKED)