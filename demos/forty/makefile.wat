#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = forty
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\canvas.obj $(OUTPUTDIR)\card.obj $(OUTPUTDIR)\game.obj &
    $(OUTPUTDIR)\pile.obj $(OUTPUTDIR)\playerdg.obj $(OUTPUTDIR)\scoredg.obj $(OUTPUTDIR)\scorefil.obj

!include $(WXDIR)\src\makeprog.wat


