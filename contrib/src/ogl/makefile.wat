# OGL makefile

WXDIR = ..\..\..
EXTRACPPFLAGS=-I$(WXDIR)\contrib\include;/DPROLOGIO

LIBTARGET= $(WXDIR)\lib\ogl_w.lib

OBJECTS =  &
    $(OUTPUTDIR)\basic.obj &
    $(OUTPUTDIR)\basic2.obj &
    $(OUTPUTDIR)\canvas.obj &
    $(OUTPUTDIR)\lines.obj &
    $(OUTPUTDIR)\divided.obj &
    $(OUTPUTDIR)\constrnt.obj &
    $(OUTPUTDIR)\composit.obj &
    $(OUTPUTDIR)\drawn.obj &
    $(OUTPUTDIR)\bmpshape.obj &
    $(OUTPUTDIR)\mfutils.obj &
    $(OUTPUTDIR)\ogldiag.obj &
    $(OUTPUTDIR)\oglmisc.obj


!include $(WXDIR)\src\makelib.wat

