# Makefile for libpng
# Watcom 10a and later 32-bit protected mode flat memory model

# Adapted by Pawel Mrochen, based on makefile.msc
# For conditions of distribution and use, see copyright notice in png.h
# Assumes that zlib.lib, zconf.h, and zlib.h have been copied to ..\zlib

# To use, do "wmake -f makefile.wat"

# ------------- Watcom 10a and later -------------
MODEL=-mf
CFLAGS= $(MODEL) -fpi87 -fp5 -5r -oaeilmnrt -s -zp4 -i=..\zlib
CC=wcc386
LD=wcl386
LIB=wlib -b -c
LDFLAGS=
O=.obj

#uncomment next to put error messages in a file
#ERRFILE= >> pngerrs

# variables
OBJS1 = png$(O) pngset$(O) pngget$(O) pngrutil$(O) pngtrans$(O) pngwutil$(O)
OBJS2 = pngmem$(O) pngpread$(O) pngread$(O) pngerror$(O) pngwrite$(O)
OBJS3 = pngrtran$(O) pngwtran$(O) pngrio$(O) pngwio$(O)

all: test

png$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngset$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngget$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngread$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngpread$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngrtran$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngrutil$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngerror$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngmem$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngrio$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngwio$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngtest$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngtrans$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngwrite$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngwtran$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

pngwutil$(O): png.h pngconf.h
	$(CC) $(CFLAGS) $*.c $(ERRFILE)

libpng.lib: $(OBJS1) $(OBJS2) $(OBJS3)
	$(LIB) -n libpng.lib +$(OBJS1)
	$(LIB) libpng.lib +$(OBJS2)
	$(LIB) libpng.lib +$(OBJS3)

pngtest.exe: pngtest.obj libpng.lib
	$(LD) $(LDFLAGS) pngtest.obj libpng.lib ..\zlib\zlib.lib

test: pngtest.exe
	pngtest

# End of makefile for libpng
