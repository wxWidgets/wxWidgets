# Purpose: makefile for event example (Watcom)
# Created: 2001-01-31

WXDIR = $(%WXWIN)

PROGRAM = event
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


