#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
#
# Makefile : Builds XPM library for Watcom C++, WIN32

WXDIR = ..\..

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET   = $(WXLIB)\xpm.lib

OBJECTS = 	crbuffri.obj &
		crdatfri.obj &
		create.obj &
        crifrbuf.obj &
		crifrdat.obj &
		data.obj &
		hashtab.obj &
        misc.obj &
		parse.obj &
        rdftodat.obj &
		rdftoi.obj &
		rgb.obj &
        scan.obj &
		simx.obj &
        wrffrdat.obj &
		wrffrp.obj &
        wrffri.obj

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


