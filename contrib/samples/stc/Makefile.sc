#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..\..
TARGET=stctest
OBJECTS = $(TARGET).obj 
EXTRALIBS=$(WXDIR)\lib\stc_sc.lib
include $(WXDIR)\src\makeprog.sc
