#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=layout
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
