# Purpose: makefile for statbar example (Salford C++)
# Created: 2000-02-04

PROGRAM = statbar
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\statbar

