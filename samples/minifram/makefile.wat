# Purpose: makefile for minifram example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = minifram
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


