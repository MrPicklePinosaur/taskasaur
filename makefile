CC=gcc

make: taskasaur

parser.o: parser.c headers/parser.h
	$(CC) -c parser.c

render.o: render.c headers/render.h
	$(CC) -c render.c

taskasaur: taskasaur.c parser.o render.o
	$(CC) -o $@ $^ -lncurses -lmenu -lmd4c

clean:
	rm taskasaur *.o
