# Purpose: makefile for #NAME example (Symantec C++)
# Created: #DATE

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
INCLUDE=$(INCDIR)
TARGET=#NAME

include $(WXDIR)\src\makesc.env

#NAME.exe: #NAME.obj $(DEFFILE) #NAME.res
	*$(CC) $(LDFLAGS) -o$@ $** $(LIBS)
    *$(RC) -k #NAME.res

sc32.def:
     echo EXETYPE NT > sc32.def
     echo SUBSYSTEM WINDOWS >> sc32.def

sc16.def:
     echo NAME $(TARGET) > sc16.def
     echo EXETYPE WINDOWS >> sc16.def
     echo STUB         'WINSTUB.EXE' >> sc16.def
     echo CODE         PRELOAD MOVEABLE DISCARDABLE >> sc16.def
     echo DATA         PRELOAD MOVEABLE MULTIPLE >> sc16.def
     echo HEAPSIZE     1024 >> sc16.def
     echo STACKSIZE    8192 >> sc16.def

clean:
    -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
    -del sc32.def
    -del sc16.def

