#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\wxWindows
TARGET=arttest
OBJECTS = $(TARGET).obj artbrows.obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
