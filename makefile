CC=gcc

make: taskasaur test

taskasaur: taskasur.c
	$(CC) -lncurses -lmenu taskasaur.c -o taskasaur

test: parser.c
	$(CC) -lncurses -lmenu -lmd4c parser.c -o parser

clean:
	rm taskasaur parser
