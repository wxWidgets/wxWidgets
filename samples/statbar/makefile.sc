#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=statbar
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
