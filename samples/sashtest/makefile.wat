# Purpose: makefile for sashtest example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = sashtest
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


