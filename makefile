CPPC=g++
CC=gcc

make: main

main:
	#$(CPPC) -lncurses main.cpp -o main
	$(CC) -lncurses main.c -o main

clean:
	rm main
