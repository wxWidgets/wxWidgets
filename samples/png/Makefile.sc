#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\wxWindows
TARGET=pngdemo
OBJECTS = $(TARGET).obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
