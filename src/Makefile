.PHONY: clean all
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif

LMDBDIR = ../lib/lmdb/libraries/liblmdb
SOURCE = $(shell echo *.c)
SOURCE += $(shell echo ../lib/sds/*.c)
SOURCE += $(shell echo ../lib/fs.c/*.c)
SOURCE += $(shell echo ../lib/hash/*.c)
SOURCE += $(LMDBDIR)/mdb.c
SOURCE += $(LMDBDIR)/midl.c

OBJECT = $(SOURCE:.c=.o)
TARGET = libts.a
SHAREDTARGET = libts.so

CC = gcc

CFLAGS = -g
CFLAGS += -std=c11
CFLAGS += -D_GNU_SOURCE
CFLAGS += -Werror
CFLAGS += -Wfatal-errors
CFLAGS += -Wno-error=discarded-qualifiers
CFLAGS += -Wno-discarded-qualifiers
CFLAGS += -fpic
CFLAGS += -I $(LMDBDIR)
CFLAGS += -I ../lib/fs.c
CFLAGS += -I ../lib/sds
CFLAGS += -I ../lib/hash

ifeq ($(detected_os),Windows)
	LDLIBS = -lntdll 
	SOLIBS = -lntdll
endif

all: $(TARGET) $(SHAREDTARGET)

clean:
	rm -f $(OBJECT) $(TARGET) $(SHAREDTARGET)

$(TARGET): $(OBJECT)
	# check archive contents with `ar -t libts.a`
	ar rcs $@ $^

$(SHAREDTARGET): $(OBJECT)
	gcc -shared -o $(SHAREDTARGET) $(OBJECT)