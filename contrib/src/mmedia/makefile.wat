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

!include $(WXDIR)\src\makewat.env

THISDIR = $(WXDIR)\contrib\src\mmedia
OUTPUTDIR = $(THISDIR)\

NAME = mmedia
LNK = $(name).lnk

LIBTARGET=$(WXDIR)\lib\mmedia_w.lib

OBJECTS = cdbase.obj cdwin.obj g711.obj g721.obj g723_24.obj &
  g723_40.obj g72x.obj sndaiff.obj sndbase.obj sndcodec.obj &
  sndcpcm.obj sndfile.obj sndg72x.obj sndpcm.obj sndulaw.obj &
  sndwav.obj sndwin.obj vidbase.obj vidwin.obj

all:     $(LIBTARGET) .SYMBOLIC
    

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(SVGLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj 
    -erase *.bak
    -erase *.err 
    -erase *.pch 
    -erase $(LIBTARGET) 
    -erase *.lbc


