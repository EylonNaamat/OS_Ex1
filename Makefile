

main.o : main.c
	gcc -c main.c -o main.o

server.o : server.c
	gcc -c server.c -o server.o

main: main.o
	gcc main.o -o main

server: server.o
	gcc server.o -o server

all: main server

.PHONY: clean
clean:
	rm main server main.o server.o

