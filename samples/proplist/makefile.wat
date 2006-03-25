# Purpose: makefile for proplist example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = proplist
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


