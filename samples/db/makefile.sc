#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=dbtest
OBJECTS = $(TARGET).obj listdb.obj
include $(WXDIR)\src\makeprog.sc
