#==============================================================================
# Makefile for UnZip, UnZipSFX and fUnZip:  Unix and MS-DOS ("real" makes only)
# Version:  5.4                                                19 November 1998
#==============================================================================


# INSTRUCTIONS (such as they are):
#
# "make sunos"	-- makes UnZip in current directory on a generic SunOS 4.x Sun
# "make list"	-- lists all supported systems (targets)
# "make help"	-- provides pointers on what targets to try if problems occur
# "make wombat" -- chokes and dies if you haven't added the specifics for your
#		    Wombat 68000 (or whatever) to the systems list
#
# CF are flags for the C compiler.  LF are flags for the loader.  LF2 are more
# flags for the loader, if they need to be at the end of the line instead of at
# the beginning (for example, some libraries).  FL and FL2 are the corre-
# sponding flags for fUnZip.  LOCAL_UNZIP is an environment variable that can
# be used to add default C flags to your compile without editing the Makefile
# (e.g., -DDEBUG_STRUC, or -FPi87 on PCs using Microsoft C).
#
# Some versions of make do not define the macro "$(MAKE)"; this is rare, but
# if things don't work, try using "make" instead of "$(MAKE)" in your system's
# makerule.  Or try adding the following line to your .login file:
#	setenv MAKE "make"
# (That never works--makes that are too stupid to define MAKE are also too
# stupid to look in the environment--but try it anyway for kicks. :-) )
#
# Memcpy and memset are provided for those systems that don't have them; they
# are in fileio.c and will be used if -DZMEM is included in CF.  These days
# almost all systems have them.
#
# Be sure to test your new UnZip (and UnZipSFX and fUnZip); successful compila-
# tion does not always imply a working program.


#####################
# MACRO DEFINITIONS #
#####################

# Defaults most systems use (use LOCAL_UNZIP in environment to add flags,
# such as -DDOSWILD).

# UnZip flags
CC = cc#	try using "gcc" target rather than changing this (CC and LD
LD = $(CC)#	must match, else "unresolved symbol:  ___main" is possible)
AS = as
LOC = $(LOCAL_UNZIP)
AF = $(LOC)
CF = -O -g -I. -I.. -I../inczip $(LOC) -I/usr/local//include  -I/usr/X11R6/include -I/usr/local/lib/glib/include -DSTRICT  -D__WXGTK__ -DGTK_NO_CHECK_CASTS -D_REENTRANT -D_IODBC_ -Wall
LF = -o sfx -L/usr/local/lib -L/usr/X11R6/lib -L/usr -lwx_gtk -lpng -ljpeg -lstdc++ -lgcc  -lc_r  -lgtk -lgdk -lgmodule -lglib -lXext -lX11 -lm -lgthread
LF2 = -s

# general-purpose stuff
#CP = cp
CP = ln
LN = ln
RM = rm -f
CHMOD = chmod
BINPERMS = 755
MANPERMS = 644
STRIP = strip
E =
O = .o
M = unix
SHELL = /bin/sh

# defaults for crc32 stuff and system dependent headers
CRC32 = crc32

# object files
OBJS1 = unzip$O $(CRC32)$O crctab$O crypt$O envargs$O explode$O
OBJS2 = extract$O fileio$O globals$O inflate$O list$O match$O
OBJS3 = process$O ttyio$O unreduce$O unshrink$O zipinfo$O
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $M$O install$O wxmain$O instsup$O
UNZIP_H = ../inczip/unzip.h ../inczip/unzpriv.h ../inczip/globals.h

# installation
# (probably can change next two to `install' and `install -d' if you have it)
INSTALL = cp
INSTALL_D = mkdir -p

###############################################
# BASIC COMPILE INSTRUCTIONS AND DEPENDENCIES #
###############################################

# this is for GNU make; comment out and notify zip-bugs if it causes errors
.SUFFIXES:	.c .o .obj .pic.o

# yes, we should be able to use the $O macro to combine these two, but it
# fails on some brain-damaged makes (e.g., AIX's)...no big deal
.c.o:
	$(CC) -c $(CF) $*.c

.c.obj:
	$(CC) -c $(CF) $*.c

.c.pic.o:
	$(CC) -c $(CF) -o $@ $*.c

all:	        sfx


# EDIT HERE FOR PARALLEL MAKES on Sequent (and others?)--screws up MS-DOS
# make utilities if default:  change "unzip$E:" to "unzip$E:&"

sfx$E:	$(OBJS)			# add `&' for parallel makes
	$(LD) $(LF) $(OBJS)

crc32$O:	crc32.c $(UNZIP_H) ../inczip/zip.h
crctab$O:	crctab.c $(UNZIP_H)  ../inczip/zip.h
crypt$O:	crypt.c $(UNZIP_H)  ../inczip/zip.h  ../inczip/crypt.h  ../inczip/ttyio.h
envargs$O:	envargs.c $(UNZIP_H)
explode$O:	explode.c $(UNZIP_H)
extract$O:	extract.c $(UNZIP_H)  ../inczip/crypt.h
fileio$O:	fileio.c $(UNZIP_H)  ../inczip/crypt.h  ../inczip/ttyio.h  ../inczip/ebcdic.h
funzip$O:	funzip.c $(UNZIP_H)  ../inczip/crypt.h  ../inczip/ttyio.h  ../inczip/tables.h
globals$O:	globals.c $(UNZIP_H)
inflate$O:	inflate.c  ../inczip/inflate.h $(UNZIP_H)
list$O:		list.c $(UNZIP_H)
match$O:	match.c $(UNZIP_H)
process$O:	process.c $(UNZIP_H)
ttyio$O:	ttyio.c $(UNZIP_H)  ../inczip/zip.h  ../inczip/crypt.h  ../inczip/ttyio.h
unreduce$O:	unreduce.c $(UNZIP_H)
unshrink$O:	unshrink.c $(UNZIP_H)
unzip$O:	unzip.c $(UNZIP_H)  ../inczip/crypt.h  ../inczip/version.h  ../inczip/consts.h
zipinfo$O:	zipinfo.c $(UNZIP_H)

unix$O:		unix.c $(UNZIP_H)  ../inczip/version.h		# Unix only
	$(CC) -c $(CF)  unix.c

install$O:		../install.c	      
	$(CC) -c $(CF)  ../install.c

wxmain$O:		../wxmain.cpp
	$(CC) -c $(CF)  ../wxmain.cpp

instsup$O:	        ../instsup.cpp 
	$(CC) -c $(CF)  ../instsup.cpp



