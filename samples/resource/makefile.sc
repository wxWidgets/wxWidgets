# Symantec C++ makefile for hello example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.cc.obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

hello.exe: hello.obj hello.def hello.res
	*$(CC) $(LDFLAGS) -o$@ hello.obj hello.def $(LIBS)
        *$(RC) -k hello.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

