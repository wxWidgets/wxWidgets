#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=dragimag
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
