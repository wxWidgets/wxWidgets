# Purpose: makefile for multicell example (Salford C++)
# Created 2000-07-28

PROGRAM = mtest
OBJECTS = $(PROGRAM).obj ..\src\multicell.obj
EXTRAINC=-I..\include

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\notebook

