#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=listtest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
