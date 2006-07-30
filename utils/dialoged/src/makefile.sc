# Symantec C++ makefile for Dialog Editor

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
OBJECTS = wx_resed.obj wx_rdlg.obj wx_reswr.obj wx_repal.obj wx_rprop.obj dialoged.obj

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

dialoged.exe: $(OBJECTS) dialoged.def dialoged.res
	*$(CC) $(LDFLAGS) -o$@ $(OBJECTS) dialoged.def $(LIBS)
	*$(RC) -k dialoged.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
