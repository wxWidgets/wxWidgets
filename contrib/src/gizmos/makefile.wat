# Gizmos makefile for Watcom C++

WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

GIZMOSLIB = $(WXDIR)\lib\gizmos.lib
THISDIR = $(WXDIR)\contrib\src\gizmos

NAME = gizmos
LNK = $(name).lnk

OBJECTS =  multicell.obj splittree.obj

all: $(GIZMOSLIB)

$(GIZMOSLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(GIZMOSLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(GIZMOSLIB) *.lbc

