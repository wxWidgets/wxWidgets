## Purpose: The automake makefile for wxWindows (src/motif subdirectory)
## Author:  Phil Blecker, Vadim Zeitlin
## Version: $Id$
##
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.3
# no-dependencies

SUFFIXES = .cpp .c

DEFS = $(TOOLKIT_DEF) $(WXDEBUG_DEFINE)
LIBS = $(GUILIBS)

VPATH = .:${srcdir}:${srcdir}/xmcombo:${srcdir}/../common:${srcdir}/../generic:${srcdir}/../html:${EXTRA_VPATH}

EXTRA_DIST = "${srcdir}/../common ${srcdir}/../generic ${srcdir}"

lib_LTLIBRARIES = @WX_LIBRARY_NAME@
EXTRA_LTLIBRARIES = libwx_motif.la libwx_gtk.la libwx_msw.la

# empty GTK library
libwx_gtk_la_SOURCES =

# empty MSW/WINE library
libwx_msw_la_SOURCES =

# these are the common files which always make part of the library
libwx_motif_la_SOURCES = \
\
 extended.c \
 parser.c \
 xmcombo.c \
 gsocket.c \
 gsockmot.cpp \
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
 strconv.cpp \
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
 dlgcmn.cpp \
 docmdi.cpp \
 docview.cpp \
 dynlib.cpp \
 event.cpp \
 ffile.cpp \
 file.cpp \
 fileconf.cpp \
 filesys.cpp \
 fs_inet.cpp \
 fs_zip.cpp \
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
 odbc.cpp \
 paper.cpp \
 prntbase.cpp \
 process.cpp \
 protocol.cpp \
 resource.cpp \
 sckaddr.cpp \
 sckfile.cpp \
 sckipc.cpp \
 sckstrm.cpp \
 serbase.cpp \
 sizer.cpp \
 socket.cpp \
 stream.cpp \
 tbarbase.cpp \
 tbarsmpl.cpp \
 textcmn.cpp \
 textfile.cpp \
 txtstrm.cpp \
 time.cpp \
 url.cpp \
 valgen.cpp \
 validate.cpp \
 valtext.cpp \
 wfstream.cpp \
 wincmn.cpp \
 wxexpr.cpp \
 unzip.c \
 zipstrm.cpp \
 zstream.cpp \
\
 busyinfo.cpp \
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
 numdlgg.cpp \
 panelg.cpp \
 printps.cpp \
 prntdlgg.cpp \
 progdlgg.cpp \
 prop.cpp \
 sashwin.cpp \
 scrolwin.cpp \
 splitter.cpp \
 statusbr.cpp \
 tabg.cpp \
 textdlgg.cpp \
 tipdlg.cpp \
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
 listbox.cpp \
 main.cpp \
 mdi.cpp \
 menu.cpp \
 menuitem.cpp \
 minifram.cpp \
 msgdlg.cpp \
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
 stattext.cpp \
 textctrl.cpp \
 timer.cpp \
 toolbar.cpp \
 utils.cpp \
 window.cpp \
\
 htmlcell.cpp \
 htmlfilter.cpp \
 htmlhelp.cpp \
 htmlhelp_io.cpp \
 htmlparser.cpp \
 htmltag.cpp \
 htmlwin.cpp \
 htmlwinparser.cpp \
 mod_fonts.cpp \
 mod_hline.cpp \
 mod_image.cpp \
 mod_layout.cpp \
 mod_links.cpp \
 mod_list.cpp \
 mod_pre.cpp \
 mod_tables.cpp \
 search.cpp

# propform.cpp \
# proplist.cpp \
 
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

libwx_motif_la_LDFLAGS = -rpath @libdir@ \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE)
libwx_motif_la_LIBADD = $(LTLIBOBJS)
libwx_motif_la_DEPENDENCIES = $(libwx_motif_la_LIBADD) lexer.l parser.y
