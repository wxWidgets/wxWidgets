# Makefile for zlib
# Watcom 10a

# This version of the zlib makefile was adapted by Chris Young for use
# with Watcom 10a 32-bit protected mode flat memory model.  It was created 
# for use with POV-Ray ray tracer and you may choose to edit the CFLAGS to 
# suit your needs but the -DMSDOS is required.
# -- Chris Young 76702.1655@compuserve.com

# To use, do "wmake -f makefile.wat"

# See zconf.h for details about the memory requirements.

# ------------- Watcom 10a -------------
MODEL=-mf 
CFLAGS= $(MODEL) -fpi87 -fp5 -zp4 -5r -w5 -oneatx -DWIN32
CC=wcc386
LD=wcl386
LIB=wlib -b -c 
LDFLAGS= 
O=.obj
LIBTARGET=..\..\lib\zlib.lib

# variables
OBJECTS=adler32$(O) compress$(O) crc32$(O) gzio$(O) uncompr$(O) deflate$(O) &
        trees$(O) zutil$(O) inflate$(O) infblock$(O) inftrees$(O) infcodes$(O) &
        infutil$(O) inffast$(O)

# all: test

all: $(LIBTARGET)

adler32.obj: adler32.c zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

compress.obj: compress.c zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

crc32.obj: crc32.c zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

deflate.obj: deflate.c deflate.h zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

gzio.obj: gzio.c zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

infblock.obj: infblock.c zutil.h zlib.h zconf.h infblock.h inftrees.h &
  infcodes.h infutil.h
	$(CC) $(CFLAGS) $*.c

infcodes.obj: infcodes.c zutil.h zlib.h zconf.h inftrees.h infutil.h &
  infcodes.h inffast.h
	$(CC) $(CFLAGS) $*.c

inflate.obj: inflate.c zutil.h zlib.h zconf.h infblock.h
	$(CC) $(CFLAGS) $*.c

inftrees.obj: inftrees.c zutil.h zlib.h zconf.h inftrees.h
	$(CC) $(CFLAGS) $*.c

infutil.obj: infutil.c zutil.h zlib.h zconf.h inftrees.h infutil.h
	$(CC) $(CFLAGS) $*.c

inffast.obj: inffast.c zutil.h zlib.h zconf.h inftrees.h infutil.h inffast.h
	$(CC) $(CFLAGS) $*.c

trees.obj: trees.c deflate.h zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

uncompr.obj: uncompr.c zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

zutil.obj: zutil.c zutil.h zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

example.obj: example.c zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

minigzip.obj: minigzip.c zlib.h zconf.h
	$(CC) $(CFLAGS) $*.c

$(LIBTARGET) : $(OBJECTS)
	%create tmp.lbc
	@for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
	wlib /b /c /n /p=512 $^@ @tmp.lbc


example.exe: example.obj $(LIBTARGET)
	$(LD) $(LDFLAGS) example.obj $(LIBTARGET)

minigzip.exe: minigzip.obj $(LIBTARGET)
	$(LD) $(LDFLAGS) minigzip.obj $(LIBTARGET)

test: minigzip.exe example.exe
	example
	echo hello world | minigzip | minigzip -d >test
	type test

clean: .SYMBOLIC
	-erase *.obj
	-erase *.exe
        -erase $(LIBTARGET)
