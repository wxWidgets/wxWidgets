# Purpose: makefile for splitter example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = splitter
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


