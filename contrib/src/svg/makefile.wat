# svg makefile for Watcom C++

WXDIR = ..\..\..
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

!include $(WXDIR)\src\makewat.env

SVGLIB = $(WXDIR)\lib\svg.lib
THISDIR = $(WXDIR)\contrib\src\svg
OUTPUTDIR = $(THISDIR)\

NAME = svg
LNK = $(name).lnk

OBJECTS = dcsvg.obj 

all: $(SVGLIB)

$(SVGLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(SVGLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(SVGLIB) *.lbc

