# Purpose: makefile for calendar example (Watcom)
# Created: 2000-01-03

WXDIR = $(%WXWIN)

PROGRAM = calendar
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


