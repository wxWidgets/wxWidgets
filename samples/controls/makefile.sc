;    Last change:  JS   10 Jul 98   11:33 am
# Symantec C++ makefile for controls example
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

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

controls.exe: controls.obj controls.def controls.res
	*$(CC) $(LDFLAGS) -o$@ $** $(LIBS)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
