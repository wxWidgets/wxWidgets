# Objects makefile

WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

EXTRACPPFLAGS=/DPROLOGIO

OBJECTSLIB = $(WXDIR)\utils\objects\lib\graphics.lib
THISDIR = $(WXDIR)\utils\objects\src

NAME = graphics
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC) -i=..\..\mfutils\src -i=..\..\prologio\src

OBJECTS =  basic.obj basic2.obj canvas.obj lines.obj misc.obj divided.obj constrnt.obj composit.obj drawn.obj bitmap.obj

all: $(OBJECTSLIB)

$(OBJECTSLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(OBJECTSLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(OBJECTSLIB) *.lbc



