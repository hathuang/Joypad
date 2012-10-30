CC=gcc
CFLAGS =-I. -Wall -O2
#CFLAGS +=-DDebug -g
DEPS = log.h
OBJ = event.o log.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

joypad: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *~
	rm -f core
