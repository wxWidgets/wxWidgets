#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=printing
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
