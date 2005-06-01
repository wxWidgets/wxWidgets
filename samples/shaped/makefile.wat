# Purpose: makefile for shaped example (Watcom)
# Created: 2000-01-03

WXDIR = $(%WXWIN)

PROGRAM = shaped
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


