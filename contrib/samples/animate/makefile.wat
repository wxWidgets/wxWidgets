#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
THISDIR = $(WXDIR)\contrib\samples\animate

PROGRAM = anitest
EXTRALIBS = $(WXDIR)\lib\anim_w.lib
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 

!include $(WXDIR)\src\makeprog.wat


