# Purpose: makefile for SVG example (Watcom)
# Created 2000-07-28

WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
PROGRAM = stctest
EXTRALIBS = $(WXDIR)\lib\stc_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\edit.obj $(OUTPUTDIR)\prefs.obj

!include $(WXDIR)\src\makeprog.wat


