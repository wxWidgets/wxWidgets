#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = griddemo
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat


