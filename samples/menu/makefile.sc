#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=menu
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
