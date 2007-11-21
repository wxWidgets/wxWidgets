# Purpose: makefile for calendar example (Salford C++)
# Created: 2000-01-03

PROGRAM = calendar
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\calendar

