#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = wxrcedit
EXTRALIBS = $(WXDIR)\lib\wxxml.lib
OBJECTS = edapp.obj editor.obj nodehnd.obj prophnd.obj xmlhelpr.obj

!include $(WXDIR)\src\makeprog.wat

