CC=gcc

make: taskasaur

taskasaur:
	$(CC) -lncurses taskasaur.c -o taskasaur

clean:
	rm taskasaur
