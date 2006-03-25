#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1999
#
# Makefile : Builds MMedia library for Watcom C++, WIN32

WXDIR = $(%WXWIN)

LIBTARGET=$(WXDIR)\contrib\lib\mmedia.lib

OBJECTS = cdbase.obj cdwin.obj g711.obj g721.obj g723_24.obj &
  g723_40.obj g72x.obj sndaiff.obj sndbase.obj sndcodec.obj &
  sndcpcm.obj sndfile.obj sndg72x.obj sndpcm.obj sndulaw.obj &
  sndwav.obj sndwin.obj vidbase.obj vidwin.obj

!include $(WXDIR)\src\makelib.wat


