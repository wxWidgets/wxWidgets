#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = forty
OBJECTS = $(PROGRAM).obj canvas.obj card.obj game.obj pile.obj playerdg.obj scoredg.obj scorefil.obj

!include $(WXDIR)\src\makeprog.wat


