# Purpose: makefile for thread example (Watcom)
# Created: 2000-03-15

WXDIR = $(%WXWIN)

PROGRAM = thread
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


