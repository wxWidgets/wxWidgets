# Purpose: makefile for toolbar example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = toolbar
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


