#
# File:		makefile.sl
# Author:	Julian Smart
# Created:	1998
#
# Makefile : Builds a wxWindows sample for Salford C++, WIN32

PROGRAM = minimal
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\minimal

