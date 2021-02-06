
CC=gcc
CFLAGS=-g -Wall -Wno-unused-variable -Wno-switch

make: taskasaur

all: taskasaur

%.o: %.c headers/%.h config.h 
	$(CC) $(CFLAGS) -c $<

taskasaur: taskasaur.c menu.o parser.o render.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ -lncurses -lmd4c

clean:
	rm taskasaur *.o
