# Symantec DigitalMars  C++ makefile

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
INCLUDE=$(INCDIR)

include $(WXDIR)\src\makesc.env

$(TARGET).exe: $(TARGET).obj $(TARGET).res
	link $(LDFLAGS) /DELEXECUTABLE /RC $*, $@, $*, $(LIBS)
    

sc32.def:
     echo EXETYPE NT > sc32.def
     echo SUBSYSTEM WINDOWS >> sc32.def

clean:
    -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
    -del sc16.def
