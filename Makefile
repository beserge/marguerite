CC = gcc
CFLAGS  = -g -Wall

all: callback.o
	$(CC) $(CFLAGS) test.c -o test
	
callback.o: callback.c
	$(CC) $(CFLAGS) -c callback.c

clean:
	rm *.o *.exe