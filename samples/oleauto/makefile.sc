#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=oleauto
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
