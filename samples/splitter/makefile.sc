#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=splitter
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
