#
# Makefile for WATCOM
#
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = tex2rtf
OBJECTS = $(OUTPUTDIR)\tex2rtf.obj $(OUTPUTDIR)\tex2any.obj $(OUTPUTDIR)\texutils.obj &
    $(OUTPUTDIR)\rtfutils.obj $(OUTPUTDIR)\xlputils.obj $(OUTPUTDIR)\htmlutil.obj  &
    $(OUTPUTDIR)\readshg.obj $(OUTPUTDIR)\table.obj

!include $(WXDIR)\src\makeprog.wat


