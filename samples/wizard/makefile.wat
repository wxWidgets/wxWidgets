# Purpose: makefile for wizard example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = wizard
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


