# Purpose: makefile for joytest example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = joytest
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


