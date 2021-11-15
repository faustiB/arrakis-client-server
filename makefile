all: fase1

arrakis.o: arrakis.c fremen.h
	gcc -c arrakis.c -Wall -Wextra

fremen.o: fremen.c fremen.h
	gcc -c fremen.c -Wall -Wextra

fase1: arrakis.o fremen.o
	gcc arrakis.o fremen.o -o fremen -Wall -Wextra
