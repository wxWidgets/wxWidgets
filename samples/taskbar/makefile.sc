#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=tbtest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
