#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = mmboard
OBJECTS = $(PROGRAM).obj mmbman.obj

!include $(WXDIR)\src\makeprog.wat

