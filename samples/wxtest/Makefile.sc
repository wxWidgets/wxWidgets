#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=test
OBJECTS = $(TARGET).obj test_wdr.obj
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
