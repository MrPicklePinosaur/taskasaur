CC=g++

make: main

main:
	$(CC) -lncurses main.cpp -o main

clean:
	rm main
