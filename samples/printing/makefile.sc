#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=printing
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
