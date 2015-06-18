INCLUDE_DIR =../include
LIB_DIR=../lib
CC=gcc
CFLAGS=-Wall -lpru -L$(LIB_DIR) -I$(INCLUDE_DIR) 
all: write poll

write: write.c
	$(CC) write.c -o write $(CFLAGS) 

poll: poll.c
	$(CC) poll.c -o poll $(CFLAGS)

clean:
	rm -f poll write 

