#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=server
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
