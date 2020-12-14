CC=gcc

make: taskasaur

taskasaur:
	$(CC) -lncurses -lmenu taskasaur.c -o taskasaur

clean:
	rm taskasaur
