#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=

TARGET=joytest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
