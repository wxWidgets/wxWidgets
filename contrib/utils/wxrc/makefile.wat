#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = wxrc
EXTRALIBS = $(WXDIR)\lib\wxxrc.lib
OBJECTS = $(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat

