#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=scroll
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
