#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=font
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
