# Purpose: makefile for #NAME example (Watcom)
# Created: #DATE

WXDIR = $(%WXWIN)

PROGRAM = #NAME
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


