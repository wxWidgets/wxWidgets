#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\utils\wxrc

PROGRAM = wxrc
EXTRALIBS = $(WXDIR)\lib\wxxrc_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj

!include $(WXDIR)\src\makeprog.wat

