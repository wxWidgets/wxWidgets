# Symantec C++ makefile for the tree library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

include $(WXDIR)\src\makesc.env

TREEDIR = $(WXDIR)\utils\wxtree
TREEINC = $(TREEDIR)\src
TREELIB = $(TREEDIR)\lib\wxtree.lib

DOCDIR = $(TREEDIR)\docs
SRCDIR = $(TREEDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(TREELIB) $(WXLIB) libw.lib commdlg.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

$(TREELIB): wxtree.obj
 	-del $(TREELIB)
	*lib $(TREELIB) y wxtree.obj, nul;

wxtree.obj: wxtree.h wxtree.$(SRCSUFF)

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

test.obj: test.h wxtree.h test.$(SRCSUFF)

# Making documents
docs:   hlp xlp
hlp:    $(DOCDIR)/wxtree.hlp
xlp:    $(DOCDIR)/wxtree.xlp
rtf:    $(DOCDIR)/wxtree.rtf

$(DOCDIR)/wxtree.hlp:         $(DOCDIR)/wxtree.rtf $(DOCDIR)/wxtree.hpj
        cd $(DOCDIR)
        -erase wxtree.ph
        hc wxtree
        cd $(SRCDIR)

$(DOCDIR)/wxtree.rtf:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxtree.rtf -twice -winhelp
        cd $(SRCDIR)

$(DOCDIR)/wxtree.xlp:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxtree.xlp -twice -xlp
        cd $(SRCDIR)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
	-del $(TREELIB)
