# Purpose: makefile for grid example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = grid
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


