#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = dbtest
OBJECTS = $(PROGRAM).obj listdb.obj

!include $(WXDIR)\src\makeprog.wat


