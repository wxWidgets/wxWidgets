# Purpose: makefile for SVG example (Watcom)
# Created 2000-07-28

WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
PROGRAM = svgtest
EXTRALIBS = $(WXDIR)\lib\svg_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 

!include $(WXDIR)\src\makeprog.wat


