#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
# Changelist:	2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#
# Makefile : Builds REGEX library for Watcom C++, WIN32

WXDIR = ..\..
EXTRACPPFLAGS=-DPOSIX_MISTAKE

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET = $(WXLIB)\regex$(WATCOM_SUFFIX).lib

OBJECTS = &
		$(OUTPUTDIR)\regcomp.obj &
		$(OUTPUTDIR)\regexec.obj &
		$(OUTPUTDIR)\regerror.obj &
		$(OUTPUTDIR)\regfree.obj

all:        $(OUTPUTDIR) $(LIBTARGET) .SYMBOLIC

$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@

LBCFILE=$(OUTPUTDIR)\regex.lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    wlib /q /b /c /n /p=512 $^@ @$(LBCFILE)

clean:   .SYMBOLIC
    -erase *.obj
    -erase $(LIBTARGET)
    -erase *.pch
    -erase *.err
    -erase *.lbc

cleanall:   clean

