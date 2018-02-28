.PHONY: clean all
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

SOURCE = $(shell echo *.c) $(shell echo ../lib/sds/*.c)
HEADER = $(shell echo *.h) $(shell echo ../lib/lmdb/libraries/liblmdb/*.h) $(shell echo ../lib/sds/*.h)
OBJECT = $(SOURCE:.c=.o)
TARGET = libts.so


ifeq ($(detected_os),Windows)
	CFLAGS = -g
endif
ifeq ($(detected_os),Linux)
	CFLAGS = -fPIC -g
endif
CC = gcc
LDFLAGS = -shared
LINKED  = ../lib/lmdb/libraries/liblmdb/liblmdb.so

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