# Symantec C++ makefile for the msw objects
# called from src\makefile.sc

# configuration section (see src\makefile.sc) ###########################

WXDIR = $(WXWIN)

include ..\makesc.env

DEBUG=0

LIBTARGET = $(LIBDIR)\wx.lib

OPTIONS=

# end of configuration section ##########################################

GENDIR=$(WXDIR)\src\generic
COMMDIR=$(WXDIR)\src\common
XPMDIR=$(WXDIR)\src\xpm
OLEDIR=ole
MSWDIR=$(WXDIR)\src\msw

GENERICOBJS= \
  $(GENDIR)\choicdgg.obj \
  $(GENDIR)\gridg.obj \
  $(GENDIR)\laywin.obj \
  $(GENDIR)\panelg.obj \
  $(GENDIR)\progdlgg.obj \
  $(GENDIR)\prop.obj \
  $(GENDIR)\propform.obj \
  $(GENDIR)\proplist.obj \
  $(GENDIR)\sashwin.obj \
  $(GENDIR)\scrolwin.obj \
  $(GENDIR)\splitter.obj \
  $(GENDIR)\statusbr.obj \
  $(GENDIR)\tabg.obj \
  $(GENDIR)\textdlgg.obj

#  $(GENDIR)\imaglist.obj \
#  $(GENDIR)\treectrl.obj \
#  $(GENDIR)\listctrl.obj \
#  $(GENDIR)\notebook.obj \

# These are generic things that don't need to be compiled on MSW,
# but sometimes it's useful to do so for testing purposes.
NONESSENTIALOBJS= \
  $(GENDIR)\printps.obj \
  $(GENDIR)\prntdlgg.obj \
  $(GENDIR)\msgdlgg.obj \
  $(GENDIR)\helpxlp.obj \
  $(GENDIR)\colrdlgg.obj \
  $(GENDIR)\fontdlgg.obj

COMMONOBJS = \
  $(COMMDIR)\cmndata.obj \
  $(COMMDIR)\config.obj \
  $(COMMDIR)\dcbase.obj \
  $(COMMDIR)\docview.obj \
  $(COMMDIR)\docmdi.obj \
  $(COMMDIR)\dynarray.obj \
  $(COMMDIR)\dynlib.obj \
  $(COMMDIR)\event.obj \
  $(COMMDIR)\file.obj \
  $(COMMDIR)\filefn.obj \
  $(COMMDIR)\fileconf.obj \
  $(COMMDIR)\framecmn.obj \
  $(COMMDIR)\gdicmn.obj \
  $(COMMDIR)\image.obj \
  $(COMMDIR)\intl.obj \
  $(COMMDIR)\ipcbase.obj \
  $(COMMDIR)\helpbase.obj \
  $(COMMDIR)\layout.obj \
  $(COMMDIR)\log.obj \
  $(COMMDIR)\memory.obj \
  $(COMMDIR)\mimetype.obj \
  $(COMMDIR)\module.obj \
  $(COMMDIR)\object.obj \
  $(COMMDIR)\prntbase.obj \
  $(COMMDIR)\resource.obj \
  $(COMMDIR)\resourc2.obj \
  $(COMMDIR)\tbarbase.obj \
  $(COMMDIR)\tbarsmpl.obj \
  $(COMMDIR)\textfile.obj \
  $(COMMDIR)\timercmn.obj \
  $(COMMDIR)\utilscmn.obj \
  $(COMMDIR)\validate.obj \
  $(COMMDIR)\valgen.obj \
  $(COMMDIR)\valtext.obj \
  $(COMMDIR)\date.obj \
  $(COMMDIR)\hash.obj \
  $(COMMDIR)\list.obj \
  $(COMMDIR)\paper.obj \
  $(COMMDIR)\string.obj \
  $(COMMDIR)\time.obj \
  $(COMMDIR)\tokenzr.obj \
  $(COMMDIR)\wxexpr.obj \
  $(COMMDIR)\y_tab.obj \
  $(COMMDIR)\extended.obj \
  $(COMMDIR)\process.obj \
  $(COMMDIR)\wfstream.obj \
  $(COMMDIR)\mstream.obj \
  $(COMMDIR)\zstream.obj \
  $(COMMDIR)\stream.obj \
  $(COMMDIR)\datstrm.obj \
  $(COMMDIR)\objstrm.obj \
  $(COMMDIR)\variant.obj \
  $(COMMDIR)\wincmn.obj \
  $(COMMDIR)\wxchar.obj

# Don't compile for WIN16
#  $(COMMDIR)\socket.obj \
#  $(COMMDIR)\sckaddr.obj \
#  $(COMMDIR)\sckfile.obj \
#  $(COMMDIR)\sckipc.obj \
#  $(COMMDIR)\sckstrm.obj \
#  $(COMMDIR)\url.obj \
#  $(COMMDIR)\http.obj \
#  $(COMMDIR)\protocol.obj \

# Don't compile
#  $(COMMDIR)\db.obj \
#  $(COMMDIR)\dbtable.obj \
#  $(COMMDIR)\odbc.obj \

MSWOBJS = \
  $(MSWDIR)\accel.obj \
  $(MSWDIR)\app.obj \
  $(MSWDIR)\bitmap.obj \
  $(MSWDIR)\bmpbuttn.obj \
  $(MSWDIR)\brush.obj \
  $(MSWDIR)\button.obj \
  $(MSWDIR)\checkbox.obj \
  $(MSWDIR)\checklst.obj \
  $(MSWDIR)\caret.obj \
  $(MSWDIR)\choice.obj \
  $(MSWDIR)\clipbrd.obj \
  $(MSWDIR)\colordlg.obj \
  $(MSWDIR)\colour.obj \
  $(MSWDIR)\combobox.obj \
  $(MSWDIR)\control.obj \
  $(MSWDIR)\curico.obj \
  $(MSWDIR)\cursor.obj \
  $(MSWDIR)\data.obj \
  $(MSWDIR)\dc.obj \
  $(MSWDIR)\dcmemory.obj \
  $(MSWDIR)\dcclient.obj \
  $(MSWDIR)\dcprint.obj \
  $(MSWDIR)\dcscreen.obj \
  $(MSWDIR)\dde.obj \
  $(MSWDIR)\dialog.obj \
  $(MSWDIR)\dib.obj \
  $(MSWDIR)\dibutils.obj \
  $(MSWDIR)\dirdlg.obj \
  $(MSWDIR)\filedlg.obj \
  $(MSWDIR)\font.obj \
  $(MSWDIR)\fontdlg.obj \
  $(MSWDIR)\frame.obj \
  $(MSWDIR)\gauge95.obj \
  $(MSWDIR)\gaugemsw.obj \
  $(MSWDIR)\gdiobj.obj \
  $(MSWDIR)\helpwin.obj \
  $(MSWDIR)\icon.obj \
  $(MSWDIR)\iniconf.obj \
  $(MSWDIR)\listbox.obj \
  $(MSWDIR)\main.obj \
  $(MSWDIR)\mdi.obj \
  $(MSWDIR)\menu.obj \
  $(MSWDIR)\menuitem.obj \
  $(MSWDIR)\metafile.obj \
  $(MSWDIR)\minifram.obj \
  $(MSWDIR)\msgdlg.obj \
  $(MSWDIR)\nativdlg.obj \
  $(MSWDIR)\ownerdrw.obj \
  $(MSWDIR)\palette.obj \
  $(MSWDIR)\pen.obj \
  $(MSWDIR)\penwin.obj \
  $(MSWDIR)\printdlg.obj \
  $(MSWDIR)\printwin.obj \
  $(MSWDIR)\radiobox.obj \
  $(MSWDIR)\radiobut.obj \
  $(MSWDIR)\region.obj \
  $(MSWDIR)\registry.obj \
  $(MSWDIR)\regconf.obj \
  $(MSWDIR)\scrolbar.obj \
  $(MSWDIR)\settings.obj \
  $(MSWDIR)\slidrmsw.obj \
  $(MSWDIR)\slider95.obj \
  $(MSWDIR)\spinbutt.obj \
  $(MSWDIR)\statbmp.obj \
  $(MSWDIR)\statbox.obj \
  $(MSWDIR)\statbr95.obj \
  $(MSWDIR)\stattext.obj \
  $(MSWDIR)\tabctrl.obj \
  $(MSWDIR)\taskbar.obj \
  $(MSWDIR)\tbar95.obj \
  $(MSWDIR)\tbarmsw.obj \
  $(MSWDIR)\textctrl.obj \
  $(MSWDIR)\thread.obj \
  $(MSWDIR)\timer.obj \
  $(MSWDIR)\tooltip.obj \
  $(MSWDIR)\utils.obj \
  $(MSWDIR)\utilsexc.obj \
  $(MSWDIR)\wave.obj \
  $(MSWDIR)\window.obj \
  $(MSWDIR)\xpmhand.obj

# Need Win95 support for these
#  $(MSWDIR)\notebook.obj \
#  $(MSWDIR)\listctrl.obj \
#  $(MSWDIR)\imaglist.obj \
#  $(MSWDIR)\treectrl.obj \
#  $(OLEDIR)\droptgt.obj \
#  $(OLEDIR)\dropsrc.obj \
#  $(OLEDIR)\dataobj.obj \
#  $(OLEDIR)\oleutils.obj \
#  $(OLEDIR)\uuid.obj \
#  $(OLEDIR)\automtn.obj

# Doesn't compile with SC++ 6
#  $(MSWDIR)\joystick.obj \
#  $(MSWDIR)\pnghand.obj \

XPMOBJECTS = 	$(XPMDIR)\crbuffri.obj\
		$(XPMDIR)\crdatfri.obj\
		$(XPMDIR)\create.obj $(XPMDIR)\crifrbuf.obj\
		$(XPMDIR)\crifrdat.obj\
		$(XPMDIR)\data.obj\
		$(XPMDIR)\hashtab.obj $(XPMDIR)\misc.obj\
		$(XPMDIR)\parse.obj $(XPMDIR)\rdftodat.obj\
		$(XPMDIR)\rdftoi.obj\
		$(XPMDIR)\rgb.obj $(XPMDIR)\scan.obj\
		$(XPMDIR)\simx.obj $(XPMDIR)\wrffrdat.obj\
		$(XPMDIR)\wrffrp.obj $(XPMDIR)\wrffri.obj

# Add $(NONESSENTIALOBJS) if wanting generic dialogs, PostScript etc.
OBJECTS = $(COMMONOBJS) $(GENERICOBJS) $(MSWOBJS) # $(XPMOBJECTS)

all: $(LIBTARGET)

$(LIBTARGET): $(OBJECTS)
	-del $(LIBTARGET)
	*lib /PAGESIZE:512 $(LIBTARGET) y $(OBJECTS), nul;

clean:
	-del *.obj
    -del $(LIBTARGET)

$(COMMDIR)\y_tab.obj:     $(COMMDIR)\y_tab.c $(COMMDIR)\lex_yy.c

$(COMMDIR)\y_tab.c:     $(COMMDIR)\dosyacc.c
        copy $(COMMDIR)\dosyacc.c $(COMMDIR)\y_tab.c

$(COMMDIR)\lex_yy.c:    $(COMMDIR)\doslex.c
    copy $(COMMDIR)\doslex.c $(COMMDIR)\lex_yy.c

#$(COMMDIR)\cmndata.obj:     $(COMMDIR)\cmndata.cpp
#	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $(OPTIONS) $(COMMDIR)\cmndata.cpp -o$(COMMDIR)\cmndata.obj

