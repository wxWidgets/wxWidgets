# Dialog Editor makefile for Watcom

WXDIR = ..\..\..

# EXTRACPPFLAGS = $(CLIPSFLAGS)

!include $(WXDIR)\src\makewat.env

WXPROPDIR = $(WXDIR)\utils\wxprop
WXPROPLIB = $(WXPROPDIR)\lib\wxprop.lib
WXPROPINC = $(WXPROPDIR)\src

THISDIR = $(WXDIR)\utils\dialoged\src

NAME = dialoged
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC) -i=$(WXPROPINC)

PROGOBJECTS = dialoged.obj wx_resed.obj wx_rdlg.obj wx_reswr.obj wx_repal.obj wx_rprop.obj

all: wx wxprop erasepch $(PROGOBJECTS) dialoged.exe

wx: .SYMBOLIC
    cd $(WXDIR)\src\msw
    wmake -f makefile.wat all
    cd $(THISDIR)

wxprop: .SYMBOLIC
    cd $(WXPROPDIR)\src
    wmake -f makefile.wat all
    cd $(THISDIR)

$(name).exe : $(PROGOBJECTS) $(name).res $(LNK) $(WXDIR)\lib\wx$(LEVEL).lib $(WXPROPLIB)
    wlink @$(LNK)
    $(BINDCOMMAND) $(name).res

$(name).res :      $(name).rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) /i$(WXDIR)\include\msw /i$(WXDIR)\contrib\fafa $(name).rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) $(DEBUGINFO)
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(name)
    @%append $(LNK) file $(WXDIR)\lib\wx$(LEVEL).lib
    @%append $(LNK) file $(WXPROPLIB)
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(PROGOBJECTS)) do @%append $(LNK) file %i

cleanutils:   .SYMBOLIC
    cd $(WXPROPDIR)\src
    wmake -f makefile.wat clean
    cd $(THISDIR)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lbc *.res


