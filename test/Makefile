.PHONY: clean all
ifeq ($(OS),Windows_NT)
    detected_os := Windows
else
    detected_os := $(shell uname -s)
endif


SOURCE = $(shell echo *.c) # $(shell ls ../src/*.c | grep -v "../src/main.c")
HEADER = $(shell echo *.h) # $(shell ls ../src/*.h | grep -v "../src/main.h")
OBJECT = $(SOURCE:.c=.o)
TARGET = test

CC = gcc
CFLAGS  = -g -Wfatal-errors
LDFLAGS = 
ifeq ($(detected_os),Linux)
LINKED  = -ldl
endif

all: $(TARGET)

clean:
	rm -rf $(OBJECT) $(TARGET) "testdata"

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $(OBJECT) -o $@ $(LDFLAGS) $(LINKED)

run: $(TARGET)
	./test