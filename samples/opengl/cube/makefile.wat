#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = cube
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj
#EXTRALIBS=$(WXDIR)\lib\glc_w.lib
#EXTRACPPFLAGS=-I$(WXDIR)\utils\glcanvas\win

!include $(WXDIR)\src\makeprog.wat


