#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=dbtest
OBJECTS = $(TARGET).obj listdb.obj
include $(WXDIR)\src\makeprog.sc
