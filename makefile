CC=gcc

make: taskasaur

all: taskasaur

parser.o: parser.c headers/parser.h
	$(CC) -c parser.c

render.o: render.c headers/render.h
	$(CC) -c render.c

menu.o: menu.c headers/menu.h
	$(CC) -c menu.c

utils.o: utils.c headers/utils.h
	$(CC) -c utils.c

# include config.h as depend

taskasaur: taskasaur.c parser.o render.o menu.o utils.o
	$(CC) -o $@ $^ -lncurses -lmd4c

clean:
	rm taskasaur *.o
