#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = anitest
OBJECTS = $(PROGRAM).obj animate.obj

!include $(WXDIR)\src\makeprog.wat


