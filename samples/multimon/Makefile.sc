#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=multimon_test
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
