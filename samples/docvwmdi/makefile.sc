#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=docview
OBJECTS = $(TARGET).obj view.obj doc.obj
include $(WXDIR)\src\makeprog.sc
