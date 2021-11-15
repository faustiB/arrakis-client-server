all: practica

main.o: main.c fremen.h
	gcc -c main.c -Wall -Wextra

fremen.o: fremen.c fremen.h
	gcc -c fremen.c -Wall -Wextra

practica: main.o fremen.o
	gcc main.o fremen.o -o fremen -Wall -Wextra
