#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=client
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
