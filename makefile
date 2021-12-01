CC = gcc
EXE = stree
FLAGS = -Wall

all: stree

stree: stree.c
	$(CC) $(FLAGS) stree.c -o $(EXE)
        
.PHONY: all clean

clean:
	-rm *.o stree
