#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=dnd
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
