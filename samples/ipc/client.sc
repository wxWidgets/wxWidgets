#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
TARGET=client
OBJECTS=$(TARGET).obj
EXTRALIBS=
include $(WXDIR)\src\makeprog.sc
