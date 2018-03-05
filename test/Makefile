.PHONY: clean all run
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif


SOURCE = $(shell echo *.c) $(shell ls ../lib/cmocka/src/cmocka.c)
HEADER = $(shell echo *.h) $(shell ls ../lib/cmocka/include/cmocka.h)
MACROS = ./cmocka.def.h
OBJECT = $(SOURCE:.c=.o)
TARGET = test

CC = gcc
CFLAGS  = $(CMOCKADEFS) -imacros $(MACROS) -I ../lib/cmocka/include/ -g -Wfatal-errors
LDFLAGS =
ifeq ($(detected_os),Linux)
LINKED  = -ldl
endif

all: $(TARGET)

clean:
	rm -f $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $(OBJECT) -o $@ $(LDFLAGS) $(LINKED)

run: $(TARGET)
	./test
