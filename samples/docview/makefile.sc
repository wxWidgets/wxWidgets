# Symantec C++ makefile for docview example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

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

OBJECTS=docview.obj view.obj doc.obj

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

docview.exe: $(OBJECTS) docview.def docview.res
	*$(CC) $(LDFLAGS) -o$@ $(OBJECTS) docview.def $(LIBS)
	*$(RC) -k docview.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
