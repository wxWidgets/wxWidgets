#Digital Mars (was Symantec) C++ makefile 
WXDIR = ..\..
EXTRALIBS=
TARGET=wxlayout
OBJECTS = wxLayout.obj kbList.obj wxllist.obj wxlparser.obj wxlwindow.obj
include $(WXDIR)\src\makeprog.sc
