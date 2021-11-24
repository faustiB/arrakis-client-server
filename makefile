all: fase1 fase2

fremen.o: fremen.c fremen.h
	gcc -c fremen.c -Wall -Wextra

atreides.o: atreides.c atreides.h
	gcc -c atreides.c -Wall -Wextra -lpthread

fase1: fremen.o
	gcc fremen.o -o fremen -Wall -Wextra

fase2: atreides.o
	gcc atreides.o -o atreides -Wall -Wextra -lpthread

clean:
	rm -f *.o fremen atreides
