# Purpose: makefile for multicell example (Watcom)
# Created 2000-07-28

WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\samples\gizmos\multicell


PROGRAM = mtest
EXTRALIBS = $(WXDIR)\lib\gizmos_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 

!include $(WXDIR)\src\makeprog.wat


