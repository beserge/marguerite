CC = gcc
CFLAGS  = -g -Wall
CINCLUDES = \
-I./portaudio/include \

all: callback.o
	$(CC) $(CFLAGS) $(CINCLUDES) -o test test.c
	$(CC) $(CFLAGS) $(CINCLUDES) -o testsaw testsaw.c -lrt -lpthread -L ./portaudio/cbuild -l:libportaudio.a

callback.o: callback.c
	$(CC) $(CFLAGS) $(CINCLUDES) -c callback.c

clean:
	rm *.o *.exe