#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=arttest
OBJECTS = $(TARGET).obj artbrows.obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
