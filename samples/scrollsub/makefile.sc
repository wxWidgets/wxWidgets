#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=scrollsub
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
