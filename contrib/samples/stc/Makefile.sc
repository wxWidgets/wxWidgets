#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..\..\wxWindows
TARGET=stctest
OBJECTS = $(TARGET).obj 
EXTRALIBS = $(WXDIR)\lib\stc_sc.lib
include $(WXDIR)\src\makeprog.sc
