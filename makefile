CC=gcc

make: taskasaur test

taskasaur:
	$(CC) -lncurses -lmenu taskasaur.c -o taskasaur

test:
	$(CC) -lncurses -lmenu parser.c -o parser

clean:
	rm taskasaur parser
