# Purpose: makefile for typetest example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = typetest
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


