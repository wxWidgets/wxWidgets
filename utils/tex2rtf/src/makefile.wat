#
# Makefile for WATCOM
#
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = tex2rtf
OBJECTS = tex2rtf.obj tex2any.obj texutils.obj rtfutils.obj xlputils.obj htmlutil.obj readshg.obj table.obj

!include $(WXDIR)\src\makeprog.wat


