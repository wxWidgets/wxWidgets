# Symantec C++ makefile for the buttonbar library

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

BUTTNBARDIR = $(WXDIR)\samples\buttnbar

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

all:    test.exe

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map

