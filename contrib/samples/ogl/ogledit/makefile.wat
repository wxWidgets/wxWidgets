#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

PROGRAM = ogledit
EXTRALIBS = $(WXDIR)\lib\ogl_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\doc.obj $(OUTPUTDIR)\view.obj $(OUTPUTDIR)\palette.obj

!include $(WXDIR)\src\makeprog.wat

