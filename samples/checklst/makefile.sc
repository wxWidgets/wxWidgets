#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=checklst
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
