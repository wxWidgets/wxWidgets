#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=drawing
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
