#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=internat
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
