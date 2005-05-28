#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
TARGET=server
OBJECTS=$(TARGET).obj
EXTRALIBS=
include $(WXDIR)\src\makeprog.sc
