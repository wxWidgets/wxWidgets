# Purpose: makefile for validate example (Watcom)
# Created: 2000-03-14

WXDIR = $(%WXWIN)

PROGRAM = validate
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


