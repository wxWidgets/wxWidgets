# Purpose: makefile for #NAME example (Watcom)
# Created: #DATE

WXDIR = $(%WXWIN)

PROGRAM = #NAME
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


