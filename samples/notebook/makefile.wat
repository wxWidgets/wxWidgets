# Purpose: makefile for notebook example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = notebook
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


