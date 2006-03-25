# Purpose: makefile for makegen example (Salford C++)
# Created: 2000-01-03

PROGRAM = makegen
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\utils\makegen

