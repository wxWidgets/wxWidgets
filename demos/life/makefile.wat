#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = life
OBJECTS = $(PROGRAM).obj dialogs.obj game.obj

!include $(WXDIR)\src\makeprog.wat


