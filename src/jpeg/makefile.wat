#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
# Changelist:	2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#
# Makefile : Builds JPEG library for Watcom C++, WIN32

WXDIR = ..\..
EXTRACPPFLAGS=-i=..\zlib

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET = $(WXLIB)\jpeg$(WATCOM_SUFFIX).lib

# library object files common to compression and decompression
COMOBJECTS = &
    $(OUTPUTDIR)\jcomapi.obj &
    $(OUTPUTDIR)\jutils.obj &
    $(OUTPUTDIR)\jerror.obj &
    $(OUTPUTDIR)\jmemmgr.obj &
    $(OUTPUTDIR)\jmemnobs.obj

# compression library object files
CLIBOBJECTS = &
    $(OUTPUTDIR)\jcapimin.obj &
    $(OUTPUTDIR)\jcapistd.obj &
    $(OUTPUTDIR)\jctrans.obj &
    $(OUTPUTDIR)\jcparam.obj &
    $(OUTPUTDIR)\jdatadst.obj &
    $(OUTPUTDIR)\jcinit.obj &
    $(OUTPUTDIR)\jcmaster.obj &
    $(OUTPUTDIR)\jcmarker.obj &
    $(OUTPUTDIR)\jcmainct.obj &
    $(OUTPUTDIR)\jcprepct.obj &
    $(OUTPUTDIR)\jccoefct.obj &
    $(OUTPUTDIR)\jccolor.obj &
    $(OUTPUTDIR)\jcsample.obj &
    $(OUTPUTDIR)\jchuff.obj &
    $(OUTPUTDIR)\jcphuff.obj &
    $(OUTPUTDIR)\jcdctmgr.obj &
    $(OUTPUTDIR)\jfdctfst.obj &
    $(OUTPUTDIR)\jfdctflt.obj &
    $(OUTPUTDIR)\jfdctint.obj

# decompression library object files
DLIBOBJECTS = &
    $(OUTPUTDIR)\jdapimin.obj &
    $(OUTPUTDIR)\jdapistd.obj &
    $(OUTPUTDIR)\jdtrans.obj &
    $(OUTPUTDIR)\jdatasrc.obj &
    $(OUTPUTDIR)\jdmaster.obj &
    $(OUTPUTDIR)\jdinput.obj &
    $(OUTPUTDIR)\jdmarker.obj &
    $(OUTPUTDIR)\jdhuff.obj &
    $(OUTPUTDIR)\jdphuff.obj &
    $(OUTPUTDIR)\jdmainct.obj &
    $(OUTPUTDIR)\jdcoefct.obj &
    $(OUTPUTDIR)\jdpostct.obj &
    $(OUTPUTDIR)\jddctmgr.obj &
    $(OUTPUTDIR)\jidctfst.obj &
    $(OUTPUTDIR)\jidctflt.obj &
    $(OUTPUTDIR)\jidctint.obj &
    $(OUTPUTDIR)\jidctred.obj &
    $(OUTPUTDIR)\jdsample.obj &
    $(OUTPUTDIR)\jdcolor.obj &
    $(OUTPUTDIR)\jquant1.obj &
    $(OUTPUTDIR)\jquant2.obj &
    $(OUTPUTDIR)\jdmerge.obj

# These objectfiles are included in libjpeg.lib
OBJECTS = $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)

all: $(OUTPUTDIR) $(LIBTARGET) .SYMBOLIC

$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@

LBCFILE=$(OUTPUTDIR)\jpeg.lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    wlib /q /b /c /n /p=512 $^@ @$(LBCFILE)

clean: .SYMBOLIC
    -erase $(OUTPUTDIR)\*.obj
    -erase $(LIBTARGET)
    -erase $(OUTPUTDIR)\*.pch
    -erase $(OUTPUTDIR)\*.err
    -erase $(OUTPUTDIR)\*.lbc

cleanall:   clean

