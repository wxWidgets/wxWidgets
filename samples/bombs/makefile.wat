#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = bombs
OBJECTS = $(PROGRAM).obj bombs1.obj game.obj

!include $(WXDIR)\src\makeprog.wat


