#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = docview
OBJECTS = $(PROGRAM).obj view.obj doc.obj

!include $(WXDIR)\src\makeprog.wat


