#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=regtest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
