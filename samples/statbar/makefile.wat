# Purpose: makefile for statbar example (Watcom)
# Created: 2000-02-04

WXDIR = $(%WXWIN)

PROGRAM = statbar
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


