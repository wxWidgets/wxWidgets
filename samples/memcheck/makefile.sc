#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=memcheck
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
