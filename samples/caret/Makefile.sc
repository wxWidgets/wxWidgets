#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\wxWindows
TARGET=caret
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
