# Gizmos makefile for Watcom C++

WXDIR = ..\..\..
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

LIBTARGET = $(WXDIR)\lib\gizmos_w.lib

OBJECTS =  &
    $(OUTPUTDIR)\multicell.obj &
    $(OUTPUTDIR)\splittree.obj &
    $(OUTPUTDIR)\editlbox.obj &
    $(OUTPUTDIR)\dynamicsash.obj &
    $(OUTPUTDIR)\ledctrl.obj

!include $(WXDIR)\src\makelib.wat


