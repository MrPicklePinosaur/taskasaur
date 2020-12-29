CC=gcc

make: taskasaur

parser.o: parser.c headers/parser.h
	$(CC) -c parser.c

taskasaur: taskasaur.c parser.o
	$(CC) -o $@ $^ -lncurses -lmenu -lmd4c

clean:
	rm taskasaur *.o
