#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = wxconvert
EXTRALIBS = $(WXDIR)\lib\wxxrc.lib $(WXDIR)\lib\wxdeprecated.lib
OBJECTS=convert.obj rc2xml.obj rc2wxr.obj wxr2xml.obj

!include $(WXDIR)\src\makeprog.wat

