# Purpose: makefile for exec example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = exec
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


