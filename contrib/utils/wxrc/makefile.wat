#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = wxrc
EXTRALIBS = $(WXDIR)\lib\wxxml.lib
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat

