#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=demo
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
