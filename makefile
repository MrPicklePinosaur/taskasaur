CC=gcc

make: taskasaur

parser.o: parser.c headers/parser.h
	$(CC) -c parser.c

render.o: render.c headers/render.h
	$(CC) -c render.c

menu.o: menu.c headers/menu.h
	$(CC) -c menu.c

taskasaur: taskasaur.c parser.o render.o menu.o
	$(CC) -o $@ $^ -lncurses -lmd4c

clean:
	rm taskasaur *.o
