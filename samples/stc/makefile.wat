#
# Makefile for WATCOM
#


WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\samples\stc
OUTPUTDIR = $(THISDIR)\

PROGRAM = stctest
EXTRALIBS = $(WXDIR)\lib\stc.lib
OBJECTS = $(PROGRAM).obj 

!include $(WXDIR)\src\makeprog.wat

