# Purpose: makefile for multicell example (Symantec C++)
# Created 2000-07-28

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
INCLUDE=$(INCDIR)
TARGET=mtest
OBJECTS = $(TARGET).obj ..\src\multicell.obj
EXTRAINC=-I..\include

include $(WXDIR)\src\makesc.env

mtest.exe: $(OBJECTS) $(DEFFILE) mtest.res
	*$(CC) $(LDFLAGS) -o$@ $** $(LIBS)
    *$(RC) -k mtest.res

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

