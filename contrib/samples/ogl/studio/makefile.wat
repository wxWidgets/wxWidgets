#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = studio
EXTRALIBS = $(WXDIR)\contrib\lib\ogl.lib
OBJECTS = $(PROGRAM).obj doc.obj shapes.obj symbols.obj view.obj cspalette.obj &
  mainfrm.obj project.obj dialogs.obj csprint.obj

!include $(WXDIR)\src\makeprog.wat

