#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=ownerdrw
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
