# Objects makefile

WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

EXTRACPPFLAGS=/DPROLOGIO

OGLLIB = $(WXDIR)\contrib\lib\ogl.lib
THISDIR = $(WXDIR)\src\ogl

NAME = ogl
LNK = $(name).lnk

OBJECTS =  basic.obj basic2.obj canvas.obj lines.obj divided.obj constrnt.obj &
    composit.obj drawn.obj bmpshape.obj mfutils.obj ogldiag.obj oglmisc.obj

all: $(OGLLIB)

$(OGLLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(OGLLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(OGLLIB) *.lbc

