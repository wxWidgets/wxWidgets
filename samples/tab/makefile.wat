# Purpose: makefile for tab example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = tab
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


