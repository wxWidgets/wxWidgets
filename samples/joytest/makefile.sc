;    Last change:  JS   12 Apr 98   10:45 am
# Symantec C++ makefile for joytest example
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

joytest.exe: joytest.obj joytest.def joytest.res
	*$(CC) $(LDFLAGS) -o$@ joytest.obj joytest.def $(LIBS)
	*$(RC) -k joytest.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
