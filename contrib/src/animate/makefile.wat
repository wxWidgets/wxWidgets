# ANIM src makefile for Watcom C++

WXDIR = ..\..\..
EXTRACPPFLAGS = -I$(WXDIR)\contrib\include

LIBTARGET = $(WXDIR)\lib\anim_w.lib

OBJECTS =  &
    $(OUTPUTDIR)\animate.obj &

!include $(WXDIR)\src\makelib.wat


