# Purpose: makefile for multicell example (Watcom)
# Created 2000-07-28

WXDIR = $(%WXWIN)

PROGRAM = mtest
OBJECTS = $(PROGRAM).obj ..\src\multicell.obj
EXTRAINC=-I..\include

!include $(WXDIR)\src\makeprog.wat


