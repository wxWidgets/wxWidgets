#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=dynamic
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
