#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=hello
$(TARGET).exe:	$(OBJECTS) $(TARGET).def $(TARGET).res
$(TARGET)
include $(WXDIR)\src\makeprog.sc
