#
# Makefile for WATCOM
#


WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\samples\svg
OUTPUTDIR = $(THISDIR)\

PROGRAM = svgtest
EXTRALIBS = $(WXDIR)\lib\svg.lib
OBJECTS = $(PROGRAM).obj 

!include $(WXDIR)\src\makeprog.wat

