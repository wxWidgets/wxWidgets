#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = ..\.. 

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
NAME = hello
LNK = $(name).lnk
OBJS = $(name).obj 

# Required for multi-threaded MFC apps
EXTRACPPFLAGS = -bm -oaxt-zp4-ei-xs-zo-w3-bm-bt=nt -d_WINDOWS -d_MBCS
refmain = _wstart2_


PRECOMP=

all:    $(name).exe

$(name).exe : $(OBJS) $(name).res $(LNK) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) -d_MBCS $(name).res

$(name).res :      $(name).rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) $(name).rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(name)
    @%append $(LNK) file $(WXLIB)\wx$(LEVEL).lib
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lnk *.res *.exe *.rex

