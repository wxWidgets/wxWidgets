# Symantec DigitalMars  C++ makefile

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
INCLUDE=$(INCDIR)

include $(WXDIR)\src\makesc.env

$(TARGET).exe: $(OBJECTS) $(TARGET).res
	link $(LDFLAGS) /DELEXECUTABLE /RC -L/exet:nt/su:windows $(OBJECTS), $@, $*, $(LIBDIR)\ $(LIBS)
    

clean:
    -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
    -del *.sym


cleanexe:
    -del *.exe

relink: cleanexe $(TARGET).exe



