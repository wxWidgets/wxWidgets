# Objects makefile

WXDIR = ..\..\..
EXTRACPPFLAGS=-I$(WXDIR)\contrib\include;/DPROLOGIO

!include $(WXDIR)\src\makewat.env

OGLLIB = $(WXDIR)\lib\ogl_w.lib
THISDIR = $(WXDIR)\contrib\src\ogl
OUTPUTDIR = $(THISDIR)\

NAME = ogl
LNK = $(name).lnk

OBJECTS =  basic.obj basic2.obj canvas.obj lines.obj divided.obj constrnt.obj &
    composit.obj drawn.obj bmpshape.obj mfutils.obj ogldiag.obj oglmisc.obj

all: $(OGLLIB) .SYMBOLIC

$(OGLLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(OGLLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj 
    -erase *.bak 
    -erase *.err 
    -erase *.pch 
    -erase $(OGLLIB) 
    -erase *.lbc

