# Purpose: makefile for propsize example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = propsize
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


