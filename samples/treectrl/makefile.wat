# Purpose: makefile for treectrl example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = treetest
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


