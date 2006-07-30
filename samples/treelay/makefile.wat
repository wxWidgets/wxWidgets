# Purpose: makefile for treelay example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = treelay
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


