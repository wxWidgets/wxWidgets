# Purpose: makefile for dragimag example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = dragimag
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


