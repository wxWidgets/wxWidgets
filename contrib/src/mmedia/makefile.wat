#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1999
#
# Makefile : Builds MMedia library for Watcom C++, WIN32

!error This will not work - there is no digitalv.h in Open Watcom as of 26 Feb 03 

WXDIR = ..\..\..
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

LIBTARGET=$(WXDIR)\lib\mmedia_w.lib

OBJECTS = &
    $(OUTPUTDIR)\cdbase.obj &
    $(OUTPUTDIR)\cdwin.obj &
    $(OUTPUTDIR)\g711.obj &
    $(OUTPUTDIR)\g721.obj &
    $(OUTPUTDIR)\g723_24.obj &
    $(OUTPUTDIR)\g723_40.obj &
    $(OUTPUTDIR)\g72x.obj &
    $(OUTPUTDIR)\sndaiff.obj &
    $(OUTPUTDIR)\sndbase.obj &
    $(OUTPUTDIR)\sndcodec.obj &
    $(OUTPUTDIR)\sndcpcm.obj &
    $(OUTPUTDIR)\sndfile.obj &
    $(OUTPUTDIR)\sndg72x.obj &
    $(OUTPUTDIR)\sndpcm.obj &
    $(OUTPUTDIR)\sndulaw.obj &
    $(OUTPUTDIR)\sndwav.obj &
    $(OUTPUTDIR)\sndwin.obj &
    $(OUTPUTDIR)\vidbase.obj &
    $(OUTPUTDIR)\vidwin.obj

!include $(WXDIR)\src\makelib.wat

