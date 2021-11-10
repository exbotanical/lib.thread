CC=gcc
CFLAGS=-g -pthread -Ideps -fPIC -Wall -Wextra -pedantic 
LDFLAGS=-shared -o

WIN_BIN=lib_thread.dll
UNIX_BIN=libthread.so

OBJFILES=$(wildcard src/*.c)  
DEPS=$(wildcard deps/*/*.c)  

TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all: unix

unix:
	$(CC) $(CFLAGS) $(DEPS) $(OBJFILES) $(LDFLAGS) $(UNIX_BIN)

win:
	$(CC) $(CFLAGS) $(DEPS) $(OBJFILES) $(LDFLAGS) $(WIN_BIN)

clean:
	rm -f $(TARGET) $(UNIX_BIN) $(WIN_BIN) main main.o 

test: 
	./scripts/test.bash 
	$(MAKE) clean

.PHONY: test clean 
