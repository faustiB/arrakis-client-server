all: clean fremen atreides

frame_config.o: frame_config.c frame_config.h
	gcc -c frame_config.c -Wall -Wextra

ioscreen.o: ioscreen.c ioscreen.h
	gcc -c ioscreen.c -Wall -Wextra

fremen.o: fremen.c fremen.h frame_config.h ioscreen.h
	gcc -c fremen.c -Wall -Wextra

atreides.o: atreides.c atreides.h frame_config.h ioscreen.h
	gcc -c atreides.c -Wall -Wextra -lpthread

fremen: fremen.o frame_config.o ioscreen.o
	gcc fremen.o frame_config.o ioscreen.o -o fremen -Wall -Wextra -ggdb3 -g

atreides: atreides.o frame_config.o ioscreen.o
	gcc atreides.o frame_config.o ioscreen.o -o atreides -Wall -Wextra -lpthread

clean:
	rm -f *.o fremen atreides

