#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
#
# Makefile : Builds PNG library for Watcom C++, WIN32

WXDIR = ..\..

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET   = $(WXLIB)\png.lib

OBJECTS = png.obj pngread.obj pngrtran.obj pngrutil.obj &
 pngpread.obj pngtrans.obj pngwrite.obj pngwtran.obj pngwutil.obj &
 pngerror.obj pngmem.obj pngwio.obj pngrio.obj pngget.obj pngset.obj

all:        $(OBJECTS) $(LIBTARGET)

$(LIBTARGET) : $(OBJECTS)
    %create tmp.lbc
    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
    wlib /b /c /n /p=512 $^@ @tmp.lbc

clean:   .SYMBOLIC
    -erase *.obj
    -erase $(LIBTARGET)
    -erase *.pch
    -erase *.err
    -erase *.lbc

cleanall:   clean

#accel.obj:     $(MSWDIR)\accel.cpp
#  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<


