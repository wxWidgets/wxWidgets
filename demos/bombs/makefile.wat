#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = bombs
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\bombs1.obj $(OUTPUTDIR)\game.obj

!include $(WXDIR)\src\makeprog.wat


