CC=gcc
CFLAGS =-I. -Wall -O2
#CFLAGS +=-DDebug -g
DEPS = syslog.h
OBJ = event.o syslog.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

input: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *~
	rm -f core
