WXDIR = $(%WXWIN)

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
LNK = $(PROGRAM).lnk

all: $(PROGRAM).exe

$(PROGRAM).exe : $(OBJECTS) $(PROGRAM).res $(LNK) $(WXLIB)\wx.lib
    wlink @$(LNK)
    $(BINDCOMMAND) $(PROGRAM).res

$(PROGRAM).res :      $(PROGRAM).rc $(WXDIR)\include\wx\msw\wx.rc
     $(RC) $(RESFLAGS1) $(PROGRAM).rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(PROGRAM).exe
    @%append $(LNK) file $(WXLIB)\wx.lib
    @for %i in ($(OBJECTS)) do @%append $(LNK) file %i

#    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
#    @%append $(LNK) $(MINDATA)
#    @%append $(LNK) $(MAXDATA)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lnk *.res *.exe

