#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = bombs
OBJECTS = $(PROGRAM).obj bombs1.obj game.obj

!include $(WXDIR)\src\makeprog.wat


