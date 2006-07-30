#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
# Changelist:	2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#
# Makefile : Builds PNG library for Watcom C++, WIN32

WXDIR = ..\..
EXTRACPPFLAGS=-i=..\zlib

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET = $(WXLIB)\png$(WATCOM_SUFFIX).lib

OBJECTS = &
    $(OUTPUTDIR)\png.obj &
    $(OUTPUTDIR)\pngread.obj &
    $(OUTPUTDIR)\pngrtran.obj &
    $(OUTPUTDIR)\pngrutil.obj &
    $(OUTPUTDIR)\pngpread.obj &
    $(OUTPUTDIR)\pngtrans.obj &
    $(OUTPUTDIR)\pngwrite.obj &
    $(OUTPUTDIR)\pngwtran.obj &
    $(OUTPUTDIR)\pngwutil.obj &
    $(OUTPUTDIR)\pngerror.obj &
    $(OUTPUTDIR)\pngmem.obj &
    $(OUTPUTDIR)\pngwio.obj &
    $(OUTPUTDIR)\pngrio.obj &
    $(OUTPUTDIR)\pngget.obj &
    $(OUTPUTDIR)\pngset.obj

all: $(OUTPUTDIR) $(LIBTARGET) .SYMBOLIC

$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@

LBCFILE=$(OUTPUTDIR)\png.lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    wlib /q /b /c /n /p=512 $^@ @$(LBCFILE)

clean:   .SYMBOLIC
    -erase $(OUTPUTDIR)\*.obj
    -erase $(LIBTARGET)
    -erase $(OUTPUTDIR)\*.pch
    -erase $(OUTPUTDIR)\*.err
    -erase $(OUTPUTDIR)\*.lbc

cleanall:   clean

