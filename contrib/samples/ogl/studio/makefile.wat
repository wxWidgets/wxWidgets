#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\samples\ogl\studio

PROGRAM = studio
EXTRALIBS = $(WXDIR)\lib\ogl_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj &
  $(OUTPUTDIR)\doc.obj &
  $(OUTPUTDIR)\shapes.obj &
  $(OUTPUTDIR)\symbols.obj &
  $(OUTPUTDIR)\view.obj &
  $(OUTPUTDIR)\cspalette.obj &
  $(OUTPUTDIR)\mainfrm.obj &
  $(OUTPUTDIR)\project.obj &
  $(OUTPUTDIR)\dialogs.obj &
  $(OUTPUTDIR)\csprint.obj

!include $(WXDIR)\src\makeprog.wat

