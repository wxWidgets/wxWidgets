## Purpose: The automake makefile for wxWindows.
## Author:  Phil Blecker, Vadim Zeitlin
## Version: $Id$
##
## Process this file with automake to produce Makefile.in

SUFFIXES = .cpp .c

DEFS = $(TOOLKIT_DEF) $(WXDEBUG_DEFINE)
LIBS = $(GUILIBS)

VPATH = .:${srcdir}/../common:${srcdir}/../generic:${EXTRA_VPATH}

EXTRA_DIST = "${srcdir}/../common ${srcdir}/../generic ${srcdir}"

lib_LTLIBRARIES = @WX_LIBRARY_NAME@
EXTRA_LTLIBRARIES = libwx_gtk2.la libwx_motif2.la

# these are the common files which always make part of the library
libwx_gtk2_la_SOURCES = \
\
 win_gtk.c \
 extended.c \
 utilsgtk.cpp \
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
 db.cpp \
 dbtable.cpp \
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
 odbc.cpp \
 paper.cpp \
 prntbase.cpp \
 process.cpp \
 protocol.cpp \
 resourc2.cpp \
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
 valgen.cpp \
 validate.cpp \
 valtext.cpp \
 wfstream.cpp \
 wincmn.cpp \
 wxexpr.cpp \
 zstream.cpp \
\
 caret.cpp \
 choicdgg.cpp \
 colrdlgg.cpp \
 dcpsg.cpp \
 dirdlgg.cpp \
 fontdlgg.cpp \
 gridg.cpp \
 helpext.cpp \
 helphtml.cpp \
 imaglist.cpp \
 laywin.cpp \
 listctrl.cpp \
 msgdlgg.cpp \
 notebook.cpp \
 panelg.cpp \
 printps.cpp \
 prntdlgg.cpp \
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
 treectrl.cpp \
\
 threadpsx.cpp \
 utilsunx.cpp \
\
 accel.cpp \
 app.cpp \
 bitmap.cpp \
 bmpbuttn.cpp \
 brush.cpp \
 button.cpp \
 checkbox.cpp \
 checklst.cpp \
 choice.cpp \
 clipbrd.cpp \
 colour.cpp \
 combobox.cpp \
 control.cpp \
 cursor.cpp \
 data.cpp \
 dataobj.cpp \
 dc.cpp \
 dcclient.cpp \
 dcmemory.cpp \
 dcscreen.cpp \
 dialog.cpp \
 dnd.cpp \
 filedlg.cpp \
 font.cpp \
 frame.cpp \
 gauge.cpp \
 gdiobj.cpp \
 icon.cpp \
 joystick.cpp \
 listbox.cpp \
 main.cpp \
 mdi.cpp \
 menu.cpp \
 minifram.cpp \
 notebook.cpp \
 palette.cpp \
 pen.cpp \
 radiobox.cpp \
 radiobut.cpp \
 region.cpp \
 scrolbar.cpp \
 settings.cpp \
 slider.cpp \
 spinbutt.cpp \
 statbmp.cpp \
 statbox.cpp \
 statline.cpp \
 stattext.cpp \
 tbargtk.cpp \
 textctrl.cpp \
 threadno.cpp \
 threadsgi.cpp \
 timer.cpp \
 tooltip.cpp \
 treectrl.cpp \
 utilsgtk.cpp \
 utilsres.cpp \
 wave.cpp \
 window.cpp

# these are the sources which we build by our own rules
#
# TODO: parser.y can be included into SOURCES, but for the sake of my life I
#       don't know where to put lexer.l - if I put it in the sources too,
#       automake tries to build lexer.lo... and fails, of course. (VZ)
BUILT_SOURCES = parser.c lexer.c
parser.c: $(srcdir)/../common/parser.y lexer.c
	$(YACC) $(srcdir)/../common/parser.y
	@sed -e "s;$(srcdir)/../common/y.tab.c;parser.y;g" < y.tab.c | \
	sed -e "s/BUFSIZ/5000/g"            | \
	sed -e "s/YYLMAX 200/YYLMAX 5000/g" | \
	sed -e "s/yy/PROIO_yy/g"            | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > parser.c
	@$(RM) y.tab.c

lexer.c: $(srcdir)/../common/lexer.l
	$(LEX) $(srcdir)/../common/lexer.l
	@sed -e "s;$(srcdir)/../common/lex.yy.c;lexer.l;g" < lex.yy.c | \
	sed -e "s/yy/PROIO_yy/g"            | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > lexer.c
	@$(RM) lex.yy.c

libwx_gtk2_la_LDFLAGS = -rpath @libdir@ -release @WX_MINOR_VERSION_NUMBER@.@WX_RELEASE_NUMBER@
libwx_gtk2_la_LIBADD = $(LTLIBOBJS)
libwx_gtk2_la_DEPENDENCIES = $(libwx_gtk2_la_LIBADD) lexer.l parser.y
