#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=nettest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
