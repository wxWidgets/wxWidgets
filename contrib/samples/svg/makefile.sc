#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..\..
EXTRALIBS=svg_sc
TARGET=svgtest
OBJECTS = $(TARGET).obj 
include $(WXDIR)\src\makeprog.sc
