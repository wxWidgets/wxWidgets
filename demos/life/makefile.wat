#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = life
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\dialogs.obj $(OUTPUTDIR)\game.obj $(OUTPUTDIR)\reader.obj

!include $(WXDIR)\src\makeprog.wat


