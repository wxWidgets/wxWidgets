# Purpose: makefile for makegen example (Watcom)
# Created: 2000-01-03

WXDIR = $(%WXWIN)

PROGRAM = makegen
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


