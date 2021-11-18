all: fase1

fremen.o: fremen.c fremen.h
	gcc -c fremen.c -Wall -Wextra

fase1: fremen.o
	gcc fremen.o -o fremen -Wall -Wextra

clean:
	rm -f *.o fremen
