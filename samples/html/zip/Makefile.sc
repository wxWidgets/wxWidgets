#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..
TARGET=zip
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
