#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=console
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
