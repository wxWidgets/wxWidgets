# Gizmos makefile for Watcom C++

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include
WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

GIZMOSLIB = $(WXDIR)\lib\gizmos_w.lib
THISDIR = $(WXDIR)\contrib\src\gizmos
OUTPUTDIR = $(THISDIR)\

NAME = gizmos
LNK = $(name).lnk

OBJECTS =  multicell.obj splittree.obj editlbox.obj dynamicsash.obj ledctrl.obj

all: $(GIZMOSLIB) .SYMBOLIC

$(GIZMOSLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(GIZMOSLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj
    -erase *.bak
    -erase *.err 
    -erase *.pch 
    -erase $(GIZMOSLIB) 
    -erase *.lbc

