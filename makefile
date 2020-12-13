CC=gcc

make: taskasaur

taskasaur:
	$(CC) -lncurses -lmenu taskasaur.c -o taskasaur

menutest:
	$(CC) -lncurses -lmenu menutest.c -o menutest

clean:
	rm taskasaur menutest
