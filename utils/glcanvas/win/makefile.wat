#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1999
#
# Makefile : Builds wxGLCanvas library for Watcom C++, WIN32

WXDIR = $(%WXWIN)

OBJECTS=glcanvas.obj
LIBTARGET=$(WXDIR)\lib\glcanvas.lib

!include $(WXDIR)\src\makelib.wat


