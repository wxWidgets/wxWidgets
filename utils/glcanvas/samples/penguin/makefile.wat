#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = cube
OBJECTS = $(PROGRAM).obj
EXTRALIBS=$(WXDIR)\lib\glcanvas.lib
EXTRACPPFLAGS=-I$(WXDIR)\utils\glcanvas\win

!include $(WXDIR)\src\makeprog.wat


