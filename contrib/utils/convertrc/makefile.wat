#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\utils\convertrc

PROGRAM = wxconvert
EXTRALIBS = $(WXDIR)\lib\wxxrc_w.lib
OBJECTS=&
    $(OUTPUTDIR)\convert.obj &
    $(OUTPUTDIR)\rc2xml.obj &
    $(OUTPUTDIR)\rc2wxr.obj &
    $(OUTPUTDIR)\wxr2xml.obj

!include $(WXDIR)\src\makeprog.wat

