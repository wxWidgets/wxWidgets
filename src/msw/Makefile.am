## Purpose: The automake makefile for wxWindows (src/msw subdirectory)
## Author:  Phil Blecker, Vadim Zeitlin
## Version: $Id$
##
## Process this file with automake to produce Makefile.in

SUFFIXES = .cpp .c

DEFS = $(TOOLKIT_DEF) $(WXDEBUG_DEFINE)
LIBS = $(GUILIBS)

VPATH = .:${srcdir}:${srcdir}/../common:${srcdir}/../generic:${EXTRA_VPATH}

EXTRA_DIST = "${srcdir}/../common ${srcdir}/../generic ${srcdir}"

lib_LTLIBRARIES = @WX_LIBRARY_NAME@
EXTRA_LTLIBRARIES = libwx_gtk.la libwx_motif.la libwx_msw.la

# these are the common files which always make part of the library
libwx_msw_la_SOURCES = \
\
 extended.c \
 parser.c \
\
 cmndata.cpp \
 dynarray.cpp \
 filefn.cpp \
 hash.cpp \
 list.cpp \
 matrix.cpp \
 memory.cpp \
 module.cpp \
 object.cpp \
 string.cpp \
 timercmn.cpp \
 utilscmn.cpp \
 tokenzr.cpp \
 variant.cpp \
 wxchar.cpp \
\
 config.cpp \
 date.cpp \
 datstrm.cpp \
 dcbase.cpp \
 docmdi.cpp \
 docview.cpp \
 dynlib.cpp \
 event.cpp \
 file.cpp \
 fileconf.cpp \
 framecmn.cpp \
 ftp.cpp \
 gdicmn.cpp \
 helpbase.cpp \
 http.cpp \
 image.cpp \
 imagbmp.cpp \
 imaggif.cpp \
 imagjpeg.cpp \
 imagpng.cpp \
 intl.cpp \
 ipcbase.cpp \
 layout.cpp \
 log.cpp \
 longlong.cpp \
 mimetype.cpp \
 mstream.cpp \
 objstrm.cpp \
 paper.cpp \
 prntbase.cpp \
 process.cpp \
 protocol.cpp \
 resource.cpp \
 sckaddr.cpp \
 sckfile.cpp \
 sckint.cpp \
 sckipc.cpp \
 sckstrm.cpp \
 serbase.cpp \
 socket.cpp \
 stream.cpp \
 tbarbase.cpp \
 tbarsmpl.cpp \
 textfile.cpp \
 time.cpp \
 url.cpp \
 validate.cpp \
 valtext.cpp \
 wfstream.cpp \
 wincmn.cpp \
 wxexpr.cpp \
 zstream.cpp \
\
 choicdgg.cpp \
 gridg.cpp \
 laywin.cpp \
 panelg.cpp \
 progdlgg.cpp \
 prop.cpp \
 propform.cpp \
 proplist.cpp \
 sashwin.cpp \
 scrolwin.cpp \
 splitter.cpp \
 statusbr.cpp \
 tabg.cpp \
 textdlgg.cpp \
\
 accel.cpp \
 app.cpp \
 bitmap.cpp \
 bmpbuttn.cpp \
 brush.cpp \
 button.cpp \
 caret.cpp \
 checkbox.cpp \
 choice.cpp \
 clipbrd.cpp \
 colordlg.cpp \
 colour.cpp \
 combobox.cpp \
 control.cpp \
 cursor.cpp \
 data.cpp \
 dc.cpp \
 dcclient.cpp \
 dcmemory.cpp \
 dcscreen.cpp \
 dcprint.cpp \
 dialog.cpp \
 filedlg.cpp \
 font.cpp \
 fontdlg.cpp \
 frame.cpp \
 gauge95.cpp \
 gdiobj.cpp \
 icon.cpp \
 imaglist.cpp \
 listbox.cpp \
 main.cpp \
 mdi.cpp \
 menu.cpp \
 menuitem.cpp \
 minifram.cpp \
 msgdlg.cpp \
 notebook.cpp \
 ownerdrw.cpp \
 palette.cpp \
 printdlg.cpp \
 printwin.cpp \
 pen.cpp \
 radiobox.cpp \
 radiobut.cpp \
 regconf.cpp \
 region.cpp \
 registry.cpp \
 scrolbar.cpp \
 settings.cpp \
 slider95.cpp \
 spinbutt.cpp \
 statbmp.cpp \
 statbox.cpp \
 stattext.cpp \
 tbar95.cpp \
 textctrl.cpp \
 thread.cpp \
 timer.cpp \
 tooltip.cpp \
 utils.cpp \
 utilsexc.cpp \
 wave.cpp \
 window.cpp \
 nativdlg.cpp

# these are the sources which we build by our own rules
#
# TODO: parser.y can be included into SOURCES, but for the sake of my life I
#       don't know where to put lexer.l - if I put it in the sources too,
#       automake tries to build lexer.lo... and fails, of course. (VZ)
BUILT_SOURCES = parser.c lexer.c

# VZ: can anyone please explain me why are we doing these substitutions?
parser.c: $(srcdir)/../common/parser.y lexer.c
	$(YACC) $(srcdir)/../common/parser.y
	@sed -e "s;$(srcdir)/../common/y.tab.c;parser.y;g" < y.tab.c | \
	sed -e "s/BUFSIZ/5000/g"            | \
	sed -e "s/YYLMAX 200/YYLMAX 5000/g" | \
	sed -e "s/yy\([^.]\)/PROIO_yy\1/g"  | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > parser.c
	@$(RM) y.tab.c

lexer.c: $(srcdir)/../common/lexer.l
	$(LEX) $(srcdir)/../common/lexer.l
	@sed -e "s;$(srcdir)/../common/lex.yy.c;lexer.l;g" < lex.yy.c | \
	sed -e "s/yy\([^.]\)/PROIO_yy\1/g"  | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > lexer.c
	@$(RM) lex.yy.c

libwx_msw_la_LDFLAGS = -rpath @libdir@ \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE) -no-undefined
libwx_msw_la_LIBADD = $(LTLIBOBJS)
libwx_msw_la_DEPENDENCIES = $(libwx_msw_la_LIBADD) lexer.l parser.y
