#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=typetest
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
