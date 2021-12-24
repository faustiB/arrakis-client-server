all: clean fase1 fase2

frame_config.o: frame_config.c frame_config.h
	gcc -c frame_config.c -Wall -Wextra

ioscreen.o: ioscreen.c ioscreen.h
	gcc -c ioscreen.c -Wall -Wextra

fremen.o: fremen.c fremen.h frame_config.h ioscreen.h
	gcc -c fremen.c -Wall -Wextra

atreides.o: atreides.c atreides.h frame_config.h ioscreen.h
	gcc -c atreides.c -Wall -Wextra -lpthread

fase1: fremen.o frame_config.o ioscreen.o
	gcc fremen.o frame_config.o ioscreen.o -o fremen -Wall -Wextra

fase2: atreides.o frame_config.o ioscreen.o
	gcc atreides.o frame_config.o ioscreen.o -o atreides -Wall -Wextra -lpthread

clean:
	rm -f *.o fremen atreides

