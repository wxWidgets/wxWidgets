#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\utils\wxrcedit

PROGRAM = wxrcedit
EXTRALIBS = $(WXDIR)\lib\wxxrc_w.lib
OBJECTS=$(OUTPUTDIR)\edapp.obj &
    $(OUTPUTDIR)\editor.obj &
    $(OUTPUTDIR)\nodehnd.obj &
    $(OUTPUTDIR)\xmlhelpr.obj &
    $(OUTPUTDIR)\preview.obj &
    $(OUTPUTDIR)\nodesdb.obj &
    $(OUTPUTDIR)\pe_basic.obj &
    $(OUTPUTDIR)\pe_adv.obj &
    $(OUTPUTDIR)\propedit.obj &
    $(OUTPUTDIR)\propframe.obj &
    $(OUTPUTDIR)\splittree.obj

!include $(WXDIR)\src\makeprog.wat

