##############################################################################
# Name:			src/zlib/makefile.wat
# Purpose:		build zlib using Watcom 11.0+ compiler
# Author:		Vadim Zeitlin
# Created:		21.01.03
# RCS-ID:		$Id$
# Copyright:		(c) 2003 Vadim Zeitlin
# Changelist:		2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..
OUTPUTDIR=watcom

!include $(WXDIR)\src\makewat.env

LIBTARGET=$(WXDIR)\lib\zlib$(WATCOM_SUFFIX).lib

# variables
OBJECTS = &
	$(OUTPUTDIR)\adler32.obj &
	$(OUTPUTDIR)\compress.obj &
	$(OUTPUTDIR)\crc32.obj &
	$(OUTPUTDIR)\gzio.obj &
	$(OUTPUTDIR)\uncompr.obj &
	$(OUTPUTDIR)\deflate.obj &
	$(OUTPUTDIR)\trees.obj &
	$(OUTPUTDIR)\zutil.obj &
	$(OUTPUTDIR)\inflate.obj &
	$(OUTPUTDIR)\infblock.obj &
	$(OUTPUTDIR)\inftrees.obj &
	$(OUTPUTDIR)\infcodes.obj &
	$(OUTPUTDIR)\infutil.obj &
	$(OUTPUTDIR)\inffast.obj

# all: test

all: $(OUTPUTDIR) $(LIBTARGET) .SYMBOLIC

$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@

LBCFILE=$(OUTPUTDIR)\zlib.lbc
$(LIBTARGET) : $(OBJECTS)
	%create $(LBCFILE)
	@for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
	wlib /q /b /c /n /p=512 $^@ @$(LBCFILE)

clean: .SYMBOLIC
	-erase *.obj
	-erase *.exe
	-erase $(LIBTARGET)

