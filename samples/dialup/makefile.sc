#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=nettest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
