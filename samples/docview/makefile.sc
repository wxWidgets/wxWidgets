#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\wxWindows
TARGET=docview
OBJECTS = $(TARGET).obj doc.obj view.obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
