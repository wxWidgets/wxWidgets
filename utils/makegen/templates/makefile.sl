# Purpose: makefile for #NAME example (Salford C++)
# Created: #DATE

PROGRAM = #NAME
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\#NAME

