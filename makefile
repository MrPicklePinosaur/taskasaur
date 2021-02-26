
CC=gcc
CFLAGS=-g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-switch
LIBS=-ltinfo -lncurses -lmd4c

make: taskasaur

all: taskasaur

%.o: %.c headers/%.h config.h 
	$(CC) $(CFLAGS) -c $< $(LIBS)

taskasaur: taskasaur.c menu.o parser.o render.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm taskasaur *.o
