#declare the variable
CC = g++

CFLAGS=-c -Wall

all: bwtsearch

bwtsearch: bwtsearch.o
	$(CC)  bwtsearch.o -o bwtsearch

bwtsearch.o:
	$(CC) $(CFLAGS) bwtsearch.cpp

.PHONY: clean
clean:
	rm -rf *o bwtsearch
