# Purpose: makefile for treectrl example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = treectrl
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


