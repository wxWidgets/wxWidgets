#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = ogledit
EXTRALIBS = $(WXDIR)\contrib\lib\ogl.lib
OBJECTS = $(PROGRAM).obj doc.obj view.obj palette.obj

!include $(WXDIR)\src\makeprog.wat

