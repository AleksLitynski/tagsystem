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
HEADER = $(shell echo *.h)
HEADER += $(shell echo ../lib/lmdb/libraries/liblmdb/*.h)
HEADER += $(shell echo ../lib/sds/*.h)
HEADER += $(shell echo ../lib/fs.c/*.h)
HEADER += $(shell echo ../lib/hash/*.h)
HEADER += $(LMDBDIR)/mdb.h
HEADER += $(LMDBDIR)/midl.h

OBJECT = $(SOURCE:.c=.o)
TARGET = libts.a

LINKED  = $(LMDBDIR)/liblmdb.a

CC = gcc

CFLAGS = -g
CFLAGS += -std=c11
CFLAGS += -D_GNU_SOURCE
CFLAGS += -Werror
CFLAGS += -Wfatal-errors
CFLAGS += -Wno-error=discarded-qualifiers
CFLAGS += -Wno-discarded-qualifiers
CFLAGS += -I $(LMDBDIR)
CFLAGS += -I ../lib/fs.c
CFLAGS += -I ../lib/sds
CFLAGS += -I ../lib/hash

ifeq ($(detected_os),Windows)
	LDLIBS = -lntdll 
	SOLIBS = -lntdll
endif

all: $(TARGET)

clean:
	rm -f $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	# check archive contents with `ar -t libts.a`
	ar rcs $@ $^
