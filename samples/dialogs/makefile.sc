#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\wxWindows.25
TARGET=dialogs
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
