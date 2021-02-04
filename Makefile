CC = gcc
CFLAGS = -g -O2 -DPA_LITTLE_ENDIAN -I$(top_srcdir)/include -I$(top_srcdir)/src/common -I$(top_srcdir)/src/os/win -DPA_USE_DS=0 -DPA_USE_WDMKS=0 -DPA_USE_ASIO=0 -DPA_USE_WASAPI=0 -DPA_USE_WMME=1 -mthreads -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DHAVE_SYS_SOUNDCARD_H=1 -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DHAVE_CLOCK_GETTIME=1 -DHAVE_NANOSLEEP=1
CINCLUDES = \
-I./portaudio/include \

all: callback.o
	$(CC) $(CFLAGS) $(CINCLUDES) -o test test.c
	$(CC) $(CFLAGS) $(CINCLUDES) -o testsaw testsaw.c -lrt -lpthread -lwinmm -L ./portaudio/cbuild -l:libportaudio.a

callback.o: callback.c
	$(CC) $(CFLAGS) $(CINCLUDES) -c callback.c

clean:
	rm *.o *.exe