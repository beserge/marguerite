CC = gcc

top_srcdir = portaudio
CFLAGS = -g -O2 -DPA_LITTLE_ENDIAN -I$(top_srcdir)/include -I./build/ -I$(top_srcdir)/src/common -I$(top_srcdir)/src/os/win -DPA_USE_DS=0 -DPA_USE_WDMKS=0 -DPA_USE_ASIO=0 -DPA_USE_WASAPI=0 -DPA_USE_WMME=1 -mthreads -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DHAVE_SYS_SOUNDCARD_H=1 -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DHAVE_CLOCK_GETTIME=1 -DHAVE_NANOSLEEP=1
LIBS = -lwinmm -lm
PALIB = libportaudio.la
LIBTOOL = /bin/sh portaudio/libtool

SOURCEDIR := src
TESTDIR   := test
BUILDDIR  := build
BINDIR    := bin

CINCLUDES = \
-I./$(SOURCEDIR) \
-I./$(BUILDDIR) \

SOURCES   := $(wildcard $(SOURCEDIR)/*.c)
OBJECTS   := $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

TESTS     := $(wildcard $(TESTDIR)/*.c)
BINARIES  := $(patsubst $(TESTDIR)/%.c, $(BINDIR)/%.exe, $(TESTS))

all: libmarguerite.a $(BINARIES)
	mkdir -p bin build

libmarguerite.a: $(OBJECTS)
	ar -rc $(BUILDDIR)/$@ $(OBJECTS)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@

$(BINDIR)/%.exe: $(TESTDIR)/%.c
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(CINCLUDES) $< -L./build/ -lmarguerite ./portaudio/lib/$(PALIB) $(LIBS) -o $@ 
#$(CC) $(CFLAGS) $(CINCLUDES) -o $@ $< $(OBJECTS) ./portaudio/lib/$(PALIB) $(LIBS) 
#$(CC) $(CFLAGS) $(CINCLUDES) $< $(OBJECTS) $(LIBS) -L ./portaudio/cbuild -l:libportaudio.a -o $@

clean:
	rm -f -- *.o *.exe ./build/*.o ./bin/*