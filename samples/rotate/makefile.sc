#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=rotate
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
