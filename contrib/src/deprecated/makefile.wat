# Objects makefile

WXDIR = ..\..\..
EXTRACPPFLAGS=-I$(WXDIR)\contrib\include;/DPROLOGIO

!include $(WXDIR)\src\makewat.env

DEPRLIB = $(WXDIR)\lib\wxdeprecated.lib
THISDIR = $(WXDIR)\contrib\src\deprecated
OUTPUTDIR = $(THISDIR)\

NAME = wxdeprecated
LNK = $(name).lnk

OBJECTS = prop.obj propform.obj proplist.obj y_tab.obj wxexpr.obj resource.obj treelay.obj

all: $(DEPRLIB) .SYMBOLIC

$(DEPRLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(DEPRLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase y_tab.c lex_yy.c *.obj *.bak *.err *.pch $(DEPRLIB) *.lbc

y_tab.obj:     y_tab.c lex_yy.c
  *$(CC) $(CFLAGS) -DUSE_DEFINE y_tab.c

y_tab.c:     dosyacc.c
        copy dosyacc.c y_tab.c

lex_yy.c:    doslex.c
    copy doslex.c lex_yy.c


