INCLUDE_DIR =../include
CC=gcc
CFLAGS=-c -Wall -I$(INCLUDE_DIR)

libpru.a: libpru.o
	ar -rcs libpru.a libpru.o

libpru.o: libpru.c
	$(CC) $(CFLAGS) libpru.c

clean:
	rm -f *.o *.a