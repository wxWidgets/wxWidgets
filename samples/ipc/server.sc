#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
TARGET=server
OBJECTS=$(TARGET).obj
EXTRALIBS=
include $(WXDIR)\src\makeprog.sc
