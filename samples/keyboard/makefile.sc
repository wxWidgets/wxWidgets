#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=keyboard
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
