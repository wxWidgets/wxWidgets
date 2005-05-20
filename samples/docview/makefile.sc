#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=docview
OBJECTS = $(TARGET).obj doc.obj view.obj 
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
