# SVG src makefile for Watcom C++

WXDIR = ..\..\..
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

LIBTARGET = $(WXDIR)\lib\svg_w.lib

OBJECTS =  &
    $(OUTPUTDIR)\dcsvg.obj &

!include $(WXDIR)\src\makelib.wat


