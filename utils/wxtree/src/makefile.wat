WXDIR = ..\..\..

NOPRECOMP=1

!include $(WXDIR)\src\makewat.env

WXLIB=$(WXDIR)\lib
LIBTARGET = ..\lib\wxtree.lib
IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)
EXTRACPPFLAGS =
NAME        = wxtree
LNK = test.lnk
TESTOBJECTS=test.obj

OBJECTS = $(name).obj

all: $(OBJECTS) $(LIBTARGET)

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

test: test.exe

test.obj:   test.$(SRCSUFF) test.h wxtree.h

test.exe : $(TESTOBJECTS) test.res $(LNK) $(LIBTARGET) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) test.res

test.res :      test.rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) test.rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name test
    @%append $(LNK) file $(WXLIB)\wx$(LEVEL).lib
    @%append $(LNK) file $(LIBTARGET)
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(TESTOBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase $(LIBTARGET) *.obj *.bak *.err *.pch *.lib *.lbc *.res *.exe


