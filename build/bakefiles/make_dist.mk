#
# "make dist" target implementation:
#


############################# Dirs #################################

WXDIR = $(top_srcdir)

# Subordinate library possibilities

SRCDIR   = $(WXDIR)/src
GENDIR   = $(WXDIR)/src/generic
COMMDIR  = $(WXDIR)/src/common
HTMLDIR  = $(WXDIR)/src/html
UNIXDIR  = $(WXDIR)/src/unix
PNGDIR   = $(WXDIR)/src/png
JPEGDIR  = $(WXDIR)/src/jpeg
TIFFDIR  = $(WXDIR)/src/tiff
ZLIBDIR  = $(WXDIR)/src/zlib
REGEXDIR = $(WXDIR)/src/regex
EXPATDIR = $(WXDIR)/src/expat
GTKDIR   = $(WXDIR)/src/gtk
X11DIR   = $(WXDIR)/src/x11
X11INC   = $(WXDIR)/include/wx/x11
MGLDIR   = $(WXDIR)/src/mgl
MOTIFDIR = $(WXDIR)/src/motif
MSWDIR   = $(WXDIR)/src/msw
PMDIR    = $(WXDIR)/src/os2
MACDIR   = $(WXDIR)/src/mac
COCOADIR = $(WXDIR)/src/cocoa
ODBCDIR  = $(WXDIR)/src/iodbc
FTDIR    = $(WXDIR)/src/freetype
INCDIR   = $(WXDIR)/include
SAMPDIR  = $(WXDIR)/samples
DEMODIR  = $(WXDIR)/demos
UTILSDIR = $(WXDIR)/utils
MISCDIR  = $(WXDIR)/misc

DOCDIR   = $(WXDIR)/docs
INTLDIR  = $(WXDIR)/locale

TOOLKITDIR = $(TOOLKIT_LOWERCASE)

########################## Archive name ###############################

# append a version suffix x.y.z to all file names
WXARCHIVE=@DISTDIR@-$(WX_VERSION).tar.gz
WXGLARCHIVE=@DISTDIR@-gl-$(WX_VERSION).tar.gz
WXSAMPLES=wx$(TOOLKIT)-samples-$(WX_VERSION).tar.gz
WXDEMOS=wx$(TOOLKIT)-demos-$(WX_VERSION).tar.gz
WXARCHIVE_ZIP=wxMSW-$(WX_VERSION).zip

WXARCHIVE_BZIP=@DISTDIR@-$(WX_VERSION).tar.bz2
WXGLARCHIVE_BZIP=@DISTDIR@-gl-$(WX_VERSION).tar.bz2
WXSAMPLES_BZIP=wx$(TOOLKIT)-samples-$(WX_VERSION).tar.bz2
WXDEMOS_BZIP=wx$(TOOLKIT)-demos-$(WX_VERSION).tar.bz2

DISTDIRNAME=@DISTDIR@-$(WX_VERSION)
DISTDIR=./_dist_dir/$(DISTDIRNAME)


########################## make dist rules ###############################

# this target should copy only the files common to really all ports (including
# wxBase) to the dist dir
ALL_DIST: distrib_clean
	mkdir _dist_dir
	mkdir $(DISTDIR)
	cp $(WXDIR)/configure.in $(DISTDIR)
	cp $(WXDIR)/configure $(DISTDIR)
	cp $(WXDIR)/autoconf_inc.m4 $(DISTDIR)
	cp $(WXDIR)/wxwin.m4 $(DISTDIR)
	cp $(WXDIR)/aclocal.m4 $(DISTDIR)
	cp $(WXDIR)/config.sub $(DISTDIR)
	cp $(WXDIR)/config.guess $(DISTDIR)
	cp $(WXDIR)/install-sh $(DISTDIR)
	cp $(WXDIR)/mkinstalldirs $(DISTDIR)
	cp $(WXDIR)/wx-config.in $(DISTDIR)
	cp $(WXDIR)/wx-config-inplace.in $(DISTDIR)
	cp $(WXDIR)/version-script.in $(DISTDIR)
	cp $(WXDIR)/setup.h.in $(DISTDIR)
	cp $(WXDIR)/setup.h_vms $(DISTDIR)
	cp $(WXDIR)/descrip.mms $(DISTDIR)
	cp $(WXDIR)/Makefile.in $(DISTDIR)
	cp $(WXDIR)/wxBase.spec $(DISTDIR)
	cp $(DOCDIR)/lgpl.txt $(DISTDIR)/COPYING.LIB
	cp $(DOCDIR)/licence.txt $(DISTDIR)/LICENCE.txt
	cp $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	mkdir $(DISTDIR)/lib
	cp $(WXDIR)/lib/vms.opt $(DISTDIR)/lib
	cp $(WXDIR)/lib/vms_gtk.opt $(DISTDIR)/lib
	mkdir $(DISTDIR)/src
	# temp hack for common/execcmn.cpp: it's not supported by tmake
	# yet (it's a header-like file but in src/common directory and it
	# shouldn't be distributed...)
	mkdir $(DISTDIR)/src/common
	cp $(SRCDIR)/common/execcmn.cpp $(DISTDIR)/src/common
	mkdir $(DISTDIR)/src/xml
	cp $(SRCDIR)/xml/*.cpp $(DISTDIR)/src/xml
	mkdir $(DISTDIR)/src/zlib
	cp $(ZLIBDIR)/*.h $(DISTDIR)/src/zlib
	cp $(ZLIBDIR)/*.c $(DISTDIR)/src/zlib
	cp $(ZLIBDIR)/README $(DISTDIR)/src/zlib
#	cp $(ZLIBDIR)/*.mms $(DISTDIR)/src/zlib
	mkdir $(DISTDIR)/src/regex
	cp $(REGEXDIR)/*.h $(DISTDIR)/src/regex
	cp $(REGEXDIR)/*.c $(DISTDIR)/src/regex
	cp $(REGEXDIR)/COPYRIGHT $(DISTDIR)/src/regex
	cp $(REGEXDIR)/README $(DISTDIR)/src/regex
	cp -Rf $(EXPATDIR) $(DISTDIR)/src
	(cd $(DISTDIR)/src/expat ; rm -rf `find -name CVS`)
	mkdir $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/*.h $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/*.c $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/*.ci $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/*.exp $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/README $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/NEWS $(DISTDIR)/src/iodbc
	cp $(ODBCDIR)/Changes.log $(DISTDIR)/src/iodbc
	# copy some files from include/ that are not installed:
	mkdir $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx
	cp $(INCDIR)/wx/fmappriv.h $(DISTDIR)/include/wx

# this target is the common part of distribution script for all GUI toolkits,
# but is not used when building wxBase distribution
ALL_GUI_DIST: ALL_DIST
	cp $(DOCDIR)/readme.txt $(DISTDIR)/README.txt
	cp $(DOCDIR)/$(TOOLKITDIR)/install.txt $(DISTDIR)/INSTALL.txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/changes.txt ; then \
	  cp $(DOCDIR)/$(TOOLKITDIR)/changes.txt $(DISTDIR)/CHANGES-$(TOOLKIT).txt ; fi
	cp $(DOCDIR)/$(TOOLKITDIR)/readme.txt $(DISTDIR)/README-$(TOOLKIT).txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/todo.txt ; then \
	  cp $(DOCDIR)/$(TOOLKITDIR)/todo.txt $(DISTDIR)/TODO.txt ; fi
	mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)
	mkdir $(DISTDIR)/include/wx/generic
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/xrc
	cp $(INCDIR)/wx/*.h $(DISTDIR)/include/wx
	cp $(INCDIR)/wx/*.inl $(DISTDIR)/include/wx
	cp $(INCDIR)/wx/*.cpp $(DISTDIR)/include/wx
	cp $(INCDIR)/wx/generic/*.h $(DISTDIR)/include/wx/generic
	cp $(INCDIR)/wx/html/*.h $(DISTDIR)/include/wx/html
	cp $(INCDIR)/wx/unix/*.h $(DISTDIR)/include/wx/unix
	cp $(INCDIR)/wx/xml/*.h $(DISTDIR)/include/wx/xml
	cp $(INCDIR)/wx/xrc/*.h $(DISTDIR)/include/wx/xrc
	cp $(INCDIR)/wx/protocol/*.h $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/art
	mkdir $(DISTDIR)/art/gtk
	mkdir $(DISTDIR)/art/motif
	cp $(WXDIR)/art/*.xpm $(DISTDIR)/art
	cp $(WXDIR)/art/gtk/*.xpm $(DISTDIR)/art/gtk
	cp $(WXDIR)/art/motif/*.xpm $(DISTDIR)/art/motif
	mkdir $(DISTDIR)/src/generic
	mkdir $(DISTDIR)/src/html
	mkdir $(DISTDIR)/src/$(TOOLKITDIR)
	mkdir $(DISTDIR)/src/png
	mkdir $(DISTDIR)/src/jpeg
	mkdir $(DISTDIR)/src/tiff
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/xrc
	cp $(SRCDIR)/xrc/*.cpp $(DISTDIR)/src/xrc
	cp $(COMMDIR)/*.cpp $(DISTDIR)/src/common
	cp $(COMMDIR)/*.c $(DISTDIR)/src/common
	cp $(COMMDIR)/*.inc $(DISTDIR)/src/common
	cp $(COMMDIR)/*.h $(DISTDIR)/src/common
	cp $(COMMDIR)/*.mms $(DISTDIR)/src/common
	cp $(UNIXDIR)/*.cpp $(DISTDIR)/src/unix
	cp $(UNIXDIR)/*.mms $(DISTDIR)/src/unix
	cp $(GENDIR)/*.cpp $(DISTDIR)/src/generic
	cp $(GENDIR)/*.mms $(DISTDIR)/src/generic
	cp $(HTMLDIR)/*.cpp $(DISTDIR)/src/html
	cp $(PNGDIR)/*.h $(DISTDIR)/src/png
	cp $(PNGDIR)/*.c $(DISTDIR)/src/png
	cp $(PNGDIR)/README $(DISTDIR)/src/png
	cp $(JPEGDIR)/*.h $(DISTDIR)/src/jpeg
	cp $(JPEGDIR)/*.c $(DISTDIR)/src/jpeg
	cp $(JPEGDIR)/README $(DISTDIR)/src/jpeg
	cp $(TIFFDIR)/*.h $(DISTDIR)/src/tiff
	cp $(TIFFDIR)/*.c $(DISTDIR)/src/tiff
	cp $(TIFFDIR)/README $(DISTDIR)/src/tiff

BASE_DIST: ALL_DIST
	# make --disable-gui the default
	sed 's/DEFAULT_wxUSE_GUI=yes/DEFAULT_wxUSE_GUI=no/' \
		$(WXDIR)/configure.in > $(DISTDIR)/configure.in
	sed 's/DEFAULT_wxUSE_GUI=yes/DEFAULT_wxUSE_GUI=no/' \
		$(WXDIR)/configure > $(DISTDIR)/configure
	chmod +x $(DISTDIR)/configure
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/msw
	cp $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	cp $(DOCDIR)/base/readme.txt $(DISTDIR)/README.txt
	cp $(WXDIR)/src/common/*.inc $(DISTDIR)/src/common
	cp $(WXDIR)/src/common/base.rc $(DISTDIR)/src/common
	cp $(WXDIR)/src/common/unzip.h $(DISTDIR)/src/common
	list='$(ALL_BASE_HEADERS)'; for p in $$list; do \
	  cp $(WXDIR)/include/$$p $(DISTDIR)/include/$$p; \
	done
	list='$(ALL_BASE_SOURCES)'; for p in $$list; do \
	  cp $(WXDIR)/$$p $(DISTDIR)/$$p; \
	done

	mkdir $(DISTDIR)/samples
	cp $(SAMPDIR)/Makefile.in $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/console.cpp $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/console.dsp $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

GTK_DIST: ALL_GUI_DIST
	cp $(WXDIR)/wxGTK.spec $(DISTDIR)
	cp $(INCDIR)/wx/gtk/*.h $(DISTDIR)/include/wx/gtk
	cp $(GTKDIR)/*.h $(DISTDIR)/src/gtk
	cp $(GTKDIR)/*.cpp $(DISTDIR)/src/gtk
	cp $(GTKDIR)/*.c $(DISTDIR)/src/gtk
	cp $(GTKDIR)/*.xbm $(DISTDIR)/src/gtk
	cp $(GTKDIR)/*.mms $(DISTDIR)/src/gtk
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

X11_DIST: ALL_GUI_DIST UNIV_DIST
	cp $(WXDIR)/wxX11.spec $(DISTDIR)
	cp $(INCDIR)/wx/x11/*.h $(DISTDIR)/include/wx/x11
	cp $(X11DIR)/*.cpp $(DISTDIR)/src/x11
	cp $(X11DIR)/*.c $(DISTDIR)/src/x11
	cp $(X11DIR)/*.xbm $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

MOTIF_DIST: ALL_GUI_DIST
	cp $(WXDIR)/wxMotif.spec $(DISTDIR)
	cp $(INCDIR)/wx/motif/*.h $(DISTDIR)/include/wx/motif
	cp $(MOTIFDIR)/*.cpp $(DISTDIR)/src/motif
	cp $(MOTIFDIR)/*.xbm $(DISTDIR)/src/motif
	mkdir $(DISTDIR)/src/motif/xmcombo
	cp $(MOTIFDIR)/xmcombo/*.c $(DISTDIR)/src/motif/xmcombo
	cp $(MOTIFDIR)/xmcombo/*.h $(DISTDIR)/src/motif/xmcombo
	cp $(MOTIFDIR)/xmcombo/copying.txt $(DISTDIR)/src/motif/xmcombo
	mkdir $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/include/wx/x11
	cp $(X11DIR)/pen.cpp $(X11DIR)/brush.cpp $(X11DIR)/utilsx.cpp \
		$(X11DIR)/bitmap.cpp $(X11DIR)/glcanvas.cpp $(X11DIR)/region.cpp \
		$(DISTDIR)/src/x11
	cp $(X11INC)/pen.h $(X11INC)/brush.h $(X11INC)/privx.h \
		$(X11INC)/bitmap.h $(X11INC)/glcanvas.h $(X11INC)/private.h $(X11INC)/region.h \
		$(DISTDIR)/include/wx/x11
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

MACX_DIST: ALL_GUI_DIST
	cp $(INCDIR)/*.* $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx/mac/carbon
	mkdir $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/carbon/private
	cp $(INCDIR)/wx/mac/*.h $(DISTDIR)/include/wx/mac
	cp $(INCDIR)/wx/mac/carbon/*.h $(DISTDIR)/include/wx/mac/carbon
	cp $(INCDIR)/wx/mac/carbon/private/*.h $(DISTDIR)/include/wx/mac/carbon/private
	cp $(INCDIR)/wx/mac/private/*.h $(DISTDIR)/include/wx/mac/private
	cp $(MACDIR)/carbon/*.cpp $(DISTDIR)/src/mac/carbon
	cp $(MACDIR)/carbon/*.c $(DISTDIR)/src/mac/carbon
	cp $(MACDIR)/carbon/*.h $(DISTDIR)/src/mac/carbon
	cp $(MACDIR)/carbon/*.r $(DISTDIR)/src/mac/carbon
	mkdir $(DISTDIR)/src/mac/carbon/morefile
	cp $(MACDIR)/carbon/morefile/*.h $(DISTDIR)/src/mac/carbon/morefile
	cp $(MACDIR)/carbon/morefile/*.c $(DISTDIR)/src/mac/carbon/morefile
	mkdir $(DISTDIR)/src/mac/carbon/morefilex
	cp $(MACDIR)/carbon/morefilex/*.h $(DISTDIR)/src/mac/carbon/morefilex
	cp $(MACDIR)/carbon/morefilex/*.c $(DISTDIR)/src/mac/carbon/morefilex
	cp $(MACDIR)/carbon/morefilex/*.cpp $(DISTDIR)/src/mac/carbon/morefilex
# obsolete
#	mkdir $(DISTDIR)/src/mac/macsock
#	cp $(MACDIR)/macsock/*.lib $(DISTDIR)/src/mac/macsock
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

# TODO: Distribute some files
COCOA_DIST: ALL_GUI_DIST
	cp $(COCOADIR)/*.mm $(DISTDIR)/src/cocoa

MSW_DIST: ALL_GUI_DIST
	cp $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw/ctl3d
	mkdir $(DISTDIR)/include/wx/msw/gnuwin32
	mkdir $(DISTDIR)/include/wx/msw/gnuwin32/gl
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	cp $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/*.cur $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/*.ico $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/*.bmp $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/ctl3d/*.h $(DISTDIR)/include/wx/msw/ctl3d
	cp $(INCDIR)/wx/msw/gnuwin32/*.h $(DISTDIR)/include/wx/msw/gnuwin32
	cp $(INCDIR)/wx/msw/gnuwin32/gl/*.h $(DISTDIR)/include/wx/msw/gnuwin32/gl
	cp $(INCDIR)/wx/msw/gnuwin32/gl/*.def $(DISTDIR)/include/wx/msw/gnuwin32/gl
	cp $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	cp $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	cp $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	cp $(MSWDIR)/*.c $(DISTDIR)/src/msw
	cp $(MSWDIR)/*.def $(DISTDIR)/src/msw
	cp $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole

MSW_ZIP_TEXT_DIST: ALL_GUI_DIST
	cp $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/msw/ctl3d
	mkdir $(DISTDIR)/include/wx/msw/gnuwin32
	mkdir $(DISTDIR)/include/wx/msw/gnuwin32/gl
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	cp $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	cp $(INCDIR)/wx/msw/ctl3d/*.h $(DISTDIR)/include/wx/msw/ctl3d
	cp $(INCDIR)/wx/msw/gnuwin32/*.h $(DISTDIR)/include/wx/msw/gnuwin32
	cp $(INCDIR)/wx/msw/gnuwin32/gl/*.h $(DISTDIR)/include/wx/msw/gnuwin32/gl
	cp $(INCDIR)/wx/msw/gnuwin32/gl/*.def $(DISTDIR)/include/wx/msw/gnuwin32/gl
	cp $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	cp $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	cp $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	cp $(MSWDIR)/*.c $(DISTDIR)/src/msw
	cp $(MSWDIR)/*.def $(DISTDIR)/src/msw
	cp $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	cp $(MSWDIR)/wince/*.* $(DISTDIR)/src/msw/wince
	cp $(SRCDIR)/*.??? $(DISTDIR)/src
	cp $(SRCDIR)/*.?? $(DISTDIR)/src
	cp $(MSWDIR)/?ake*.* $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

UNIV_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/include/wx/univ
	mkdir $(DISTDIR)/src/univ
	mkdir $(DISTDIR)/src/univ/themes
	cp $(INCDIR)/wx/univ/*.h $(DISTDIR)/include/wx/univ
	cp $(INCDIR)/wx/univ/setup0.h $(DISTDIR)/include/wx/univ/setup.h
	cp $(SRCDIR)/univ/*.cpp $(DISTDIR)/src/univ
	cp $(SRCDIR)/univ/themes/*.cpp $(DISTDIR)/src/univ/themes

MGL_DIST: UNIV_DIST
	cp $(WXDIR)/wxMGL.spec $(DISTDIR)
	cp $(INCDIR)/wx/mgl/*.h $(DISTDIR)/include/wx/mgl
	cp $(SRCDIR)/mgl/make* $(DISTDIR)/src/mgl
	cp $(SRCDIR)/mgl/*.cpp $(DISTDIR)/src/mgl
	mkdir $(DISTDIR)/contrib
	cp -R $(WXDIR)/contrib $(DISTDIR)

DEMOS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/demos
	cp $(DEMODIR)/Makefile.in $(DISTDIR)/demos

	mkdir $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/Makefile.in $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/makefile.unx $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/*.cpp $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/*.h $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/*.xpm $(DISTDIR)/demos/bombs
	cp $(DEMODIR)/bombs/readme.txt $(DISTDIR)/demos/bombs

	cp -R $(DEMODIR)/dbbrowse $(DISTDIR)/demos/dbbrowse

	mkdir $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/Makefile.in $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/makefile.unx $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/*.h $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/*.cpp $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/*.xpm $(DISTDIR)/demos/forty
	cp $(DEMODIR)/forty/*.xbm $(DISTDIR)/demos/forty

	mkdir $(DISTDIR)/demos/life
	mkdir $(DISTDIR)/demos/life/bitmaps
	cp $(DEMODIR)/life/Makefile.in $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/makefile.unx $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/*.cpp $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/*.h $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/*.xpm $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/*.inc $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/*.lif $(DISTDIR)/demos/life
	cp $(DEMODIR)/life/bitmaps/*.xpm $(DISTDIR)/demos/life/bitmaps

	mkdir $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/Makefile.in $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/makefile.unx $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/*.h $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/*.cpp $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/*.xpm $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/*.dat $(DISTDIR)/demos/poem
	cp $(DEMODIR)/poem/*.txt $(DISTDIR)/demos/poem

	mkdir $(DISTDIR)/demos/fractal
	cp $(DEMODIR)/fractal/Makefile.in $(DISTDIR)/demos/fractal
	cp $(DEMODIR)/fractal/makefile.unx $(DISTDIR)/demos/fractal
	cp $(DEMODIR)/fractal/*.cpp $(DISTDIR)/demos/fractal

SAMPLES_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/samples
	cp $(SAMPDIR)/Makefile.in $(DISTDIR)/samples
	cp $(SAMPDIR)/sample.* $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/artprov
	cp $(SAMPDIR)/artprov/Makefile.in $(DISTDIR)/samples/artprov
	cp $(SAMPDIR)/artprov/makefile.unx $(DISTDIR)/samples/artprov
	cp $(SAMPDIR)/artprov/*.cpp $(DISTDIR)/samples/artprov
	cp $(SAMPDIR)/artprov/*.h $(DISTDIR)/samples/artprov
	cp $(SAMPDIR)/artprov/*.xpm $(DISTDIR)/samples/artprov

	mkdir $(DISTDIR)/samples/calendar
	cp $(SAMPDIR)/calendar/Makefile.in $(DISTDIR)/samples/calendar
	cp $(SAMPDIR)/calendar/makefile.unx $(DISTDIR)/samples/calendar
	cp $(SAMPDIR)/calendar/*.cpp $(DISTDIR)/samples/calendar

	mkdir $(DISTDIR)/samples/caret
	cp $(SAMPDIR)/caret/Makefile.in $(DISTDIR)/samples/caret
	cp $(SAMPDIR)/caret/makefile.unx $(DISTDIR)/samples/caret
	cp $(SAMPDIR)/caret/*.cpp $(DISTDIR)/samples/caret
	cp $(SAMPDIR)/caret/*.xpm $(DISTDIR)/samples/caret

	mkdir $(DISTDIR)/samples/checklst
	cp $(SAMPDIR)/checklst/Makefile.in $(DISTDIR)/samples/checklst
	cp $(SAMPDIR)/checklst/makefile.unx $(DISTDIR)/samples/checklst
	cp $(SAMPDIR)/checklst/*.cpp $(DISTDIR)/samples/checklst
	cp $(SAMPDIR)/checklst/*.xpm $(DISTDIR)/samples/checklst
	cp $(SAMPDIR)/checklst/*.mms $(DISTDIR)/samples/checklst

	mkdir $(DISTDIR)/samples/config
	cp $(SAMPDIR)/config/Makefile.in $(DISTDIR)/samples/config
	cp $(SAMPDIR)/config/makefile.unx $(DISTDIR)/samples/config
	cp $(SAMPDIR)/config/*.cpp $(DISTDIR)/samples/config

	mkdir $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/*.cpp $(DISTDIR)/samples/console
	cp $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

	mkdir $(DISTDIR)/samples/controls
	mkdir $(DISTDIR)/samples/controls/icons
	cp $(SAMPDIR)/controls/Makefile.in $(DISTDIR)/samples/controls
	cp $(SAMPDIR)/controls/makefile.unx $(DISTDIR)/samples/controls
	cp $(SAMPDIR)/controls/*.cpp $(DISTDIR)/samples/controls
	cp $(SAMPDIR)/controls/*.xpm $(DISTDIR)/samples/controls
	cp $(SAMPDIR)/controls/*.mms $(DISTDIR)/samples/controls
	cp $(SAMPDIR)/controls/icons/*.??? $(DISTDIR)/samples/controls/icons

	mkdir $(DISTDIR)/samples/db
	cp $(SAMPDIR)/db/Makefile.in $(DISTDIR)/samples/db
	cp $(SAMPDIR)/db/makefile.unx $(DISTDIR)/samples/db
	cp $(SAMPDIR)/db/*.cpp $(DISTDIR)/samples/db
	cp $(SAMPDIR)/db/*.h $(DISTDIR)/samples/db
	cp $(SAMPDIR)/db/*.xpm $(DISTDIR)/samples/db

	mkdir $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/Makefile.in $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/makefile.unx $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/*.cpp $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/*.h $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/*.txt $(DISTDIR)/samples/dialogs
	cp $(SAMPDIR)/dialogs/*.mms $(DISTDIR)/samples/dialogs

	mkdir $(DISTDIR)/samples/dialup
	cp $(SAMPDIR)/dialup/Makefile.in $(DISTDIR)/samples/dialup
	cp $(SAMPDIR)/dialup/makefile.unx $(DISTDIR)/samples/dialup
	cp $(SAMPDIR)/dialup/*.cpp $(DISTDIR)/samples/dialup

	mkdir $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/Makefile.in $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/makefile.unx $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/*.wxr $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/*.cpp $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/*.xpm $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/*.txt $(DISTDIR)/samples/dnd
	cp $(SAMPDIR)/dnd/*.png $(DISTDIR)/samples/dnd

	mkdir $(DISTDIR)/samples/docview
	cp $(SAMPDIR)/docview/Makefile.in $(DISTDIR)/samples/docview
	cp $(SAMPDIR)/docview/makefile.unx $(DISTDIR)/samples/docview
	cp $(SAMPDIR)/docview/*.cpp $(DISTDIR)/samples/docview
	cp $(SAMPDIR)/docview/*.h $(DISTDIR)/samples/docview
	cp $(SAMPDIR)/docview/*.xpm $(DISTDIR)/samples/docview

	mkdir $(DISTDIR)/samples/docvwmdi
	cp $(SAMPDIR)/docvwmdi/Makefile.in $(DISTDIR)/samples/docvwmdi
	cp $(SAMPDIR)/docvwmdi/makefile.unx $(DISTDIR)/samples/docvwmdi
	cp $(SAMPDIR)/docvwmdi/*.cpp $(DISTDIR)/samples/docvwmdi
	cp $(SAMPDIR)/docvwmdi/*.h $(DISTDIR)/samples/docvwmdi

	mkdir $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/Makefile.in $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/makefile.unx $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/*.cpp $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/*.h $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/*.xpm $(DISTDIR)/samples/dragimag
	cp $(SAMPDIR)/dragimag/*.png $(DISTDIR)/samples/dragimag

	mkdir $(DISTDIR)/samples/drawing
	cp $(SAMPDIR)/drawing/Makefile.in $(DISTDIR)/samples/drawing
	cp $(SAMPDIR)/drawing/makefile.unx $(DISTDIR)/samples/drawing
	cp $(SAMPDIR)/drawing/*.cpp $(DISTDIR)/samples/drawing
	cp $(SAMPDIR)/drawing/*.xpm $(DISTDIR)/samples/drawing
	cp $(SAMPDIR)/drawing/*.bmp $(DISTDIR)/samples/drawing

	mkdir $(DISTDIR)/samples/dynamic
	cp $(SAMPDIR)/dynamic/Makefile.in $(DISTDIR)/samples/dynamic
	cp $(SAMPDIR)/dynamic/makefile.unx $(DISTDIR)/samples/dynamic
	cp $(SAMPDIR)/dynamic/*.cpp $(DISTDIR)/samples/dynamic
	cp $(SAMPDIR)/dynamic/*.xpm $(DISTDIR)/samples/dynamic

	mkdir $(DISTDIR)/samples/erase
	cp $(SAMPDIR)/erase/Makefile.in $(DISTDIR)/samples/erase
	cp $(SAMPDIR)/erase/makefile.unx $(DISTDIR)/samples/erase
	cp $(SAMPDIR)/erase/*.cpp $(DISTDIR)/samples/erase
	cp $(SAMPDIR)/erase/*.xml $(DISTDIR)/samples/erase

	mkdir $(DISTDIR)/samples/event
	cp $(SAMPDIR)/event/Makefile.in $(DISTDIR)/samples/event
	cp $(SAMPDIR)/event/makefile.unx $(DISTDIR)/samples/event
	cp $(SAMPDIR)/event/*.cpp $(DISTDIR)/samples/event

	mkdir $(DISTDIR)/samples/except
	cp $(SAMPDIR)/except/Makefile.in $(DISTDIR)/samples/except
	cp $(SAMPDIR)/except/*.cpp $(DISTDIR)/samples/except

	mkdir $(DISTDIR)/samples/exec
	cp $(SAMPDIR)/exec/Makefile.in $(DISTDIR)/samples/exec
	cp $(SAMPDIR)/exec/makefile.unx $(DISTDIR)/samples/exec
	cp $(SAMPDIR)/exec/*.cpp $(DISTDIR)/samples/exec

	mkdir $(DISTDIR)/samples/font
	cp $(SAMPDIR)/font/Makefile.in $(DISTDIR)/samples/font
	cp $(SAMPDIR)/font/makefile.unx $(DISTDIR)/samples/font
	cp $(SAMPDIR)/font/*.cpp $(DISTDIR)/samples/font

	mkdir $(DISTDIR)/samples/grid
	cp $(SAMPDIR)/grid/Makefile.in $(DISTDIR)/samples/grid
	cp $(SAMPDIR)/grid/makefile.unx $(DISTDIR)/samples/grid
	cp $(SAMPDIR)/grid/*.cpp $(SAMPDIR)/grid/*.h $(DISTDIR)/samples/grid

	mkdir $(DISTDIR)/samples/help
	mkdir $(DISTDIR)/samples/help/doc
	cp $(SAMPDIR)/help/Makefile.in $(DISTDIR)/samples/help
	cp $(SAMPDIR)/help/makefile.unx $(DISTDIR)/samples/help
	cp $(SAMPDIR)/help/*.cpp $(DISTDIR)/samples/help
	cp $(SAMPDIR)/help/*.xpm $(DISTDIR)/samples/help
	cp $(SAMPDIR)/help/doc/*.* $(DISTDIR)/samples/help/doc

	mkdir $(DISTDIR)/samples/htlbox
	cp $(SAMPDIR)/htlbox/Makefile.in $(DISTDIR)/samples/htlbox
	cp $(SAMPDIR)/htlbox/makefile.unx $(DISTDIR)/samples/htlbox
	cp $(SAMPDIR)/htlbox/*.cpp $(DISTDIR)/samples/htlbox
	cp $(SAMPDIR)/htlbox/*.xpm $(DISTDIR)/samples/htlbox

	mkdir $(DISTDIR)/samples/html
	cp $(SAMPDIR)/html/Makefile.in $(DISTDIR)/samples/html
	mkdir $(DISTDIR)/samples/html/about
	cp $(SAMPDIR)/html/about/Makefile.in $(DISTDIR)/samples/html/about
	cp $(SAMPDIR)/html/about/*.cpp $(DISTDIR)/samples/html/about
	mkdir $(DISTDIR)/samples/html/about/data
	cp $(SAMPDIR)/html/about/data/*.htm $(DISTDIR)/samples/html/about/data
	cp $(SAMPDIR)/html/about/data/*.png $(DISTDIR)/samples/html/about/data
	mkdir $(DISTDIR)/samples/html/help
	cp $(SAMPDIR)/html/help/Makefile.in $(DISTDIR)/samples/html/help
	cp $(SAMPDIR)/html/help/*.cpp $(DISTDIR)/samples/html/help
	mkdir $(DISTDIR)/samples/html/help/helpfiles
	cp $(SAMPDIR)/html/help/helpfiles/*.??? $(DISTDIR)/samples/html/help/helpfiles
	mkdir $(DISTDIR)/samples/html/helpview
	cp $(SAMPDIR)/html/helpview/Makefile.in $(DISTDIR)/samples/html/helpview
	cp $(SAMPDIR)/html/helpview/*.cpp $(DISTDIR)/samples/html/helpview
	cp $(SAMPDIR)/html/helpview/*.zip $(DISTDIR)/samples/html/helpview
	mkdir $(DISTDIR)/samples/html/printing
	cp $(SAMPDIR)/html/printing/Makefile.in $(DISTDIR)/samples/html/printing
	cp $(SAMPDIR)/html/printing/*.cpp $(DISTDIR)/samples/html/printing
	cp $(SAMPDIR)/html/printing/*.htm $(DISTDIR)/samples/html/printing
	mkdir $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/Makefile.in $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.cpp $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.bmp $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.png $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.gif $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.htm $(DISTDIR)/samples/html/test
	cp $(SAMPDIR)/html/test/*.html $(DISTDIR)/samples/html/test
	mkdir $(DISTDIR)/samples/html/virtual
	cp $(SAMPDIR)/html/virtual/Makefile.in $(DISTDIR)/samples/html/virtual
	cp $(SAMPDIR)/html/virtual/*.cpp $(DISTDIR)/samples/html/virtual
	cp $(SAMPDIR)/html/virtual/*.htm $(DISTDIR)/samples/html/virtual
	mkdir $(DISTDIR)/samples/html/widget
	cp $(SAMPDIR)/html/widget/Makefile.in $(DISTDIR)/samples/html/widget
	cp $(SAMPDIR)/html/widget/*.cpp $(DISTDIR)/samples/html/widget
	cp $(SAMPDIR)/html/widget/*.htm $(DISTDIR)/samples/html/widget
	mkdir $(DISTDIR)/samples/html/zip
	cp $(SAMPDIR)/html/zip/Makefile.in $(DISTDIR)/samples/html/zip
	cp $(SAMPDIR)/html/zip/*.cpp $(DISTDIR)/samples/html/zip
	cp $(SAMPDIR)/html/zip/*.htm $(DISTDIR)/samples/html/zip
	cp $(SAMPDIR)/html/zip/*.zip $(DISTDIR)/samples/html/zip

	mkdir $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/Makefile.in $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/makefile.unx $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/*.cpp $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/*.mms $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/horse.* $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/smile.xbm $(DISTDIR)/samples/image
	cp $(SAMPDIR)/image/smile.xpm $(DISTDIR)/samples/image

	mkdir $(DISTDIR)/samples/internat
	mkdir $(DISTDIR)/samples/internat/de
	mkdir $(DISTDIR)/samples/internat/fr
	mkdir $(DISTDIR)/samples/internat/ju
	mkdir $(DISTDIR)/samples/internat/ru
	cp $(SAMPDIR)/internat/Makefile.in $(DISTDIR)/samples/internat
	cp $(SAMPDIR)/internat/makefile.unx $(DISTDIR)/samples/internat
	cp $(SAMPDIR)/internat/*.cpp $(DISTDIR)/samples/internat
	cp $(SAMPDIR)/internat/*.xpm $(DISTDIR)/samples/internat
	cp $(SAMPDIR)/internat/*.txt $(DISTDIR)/samples/internat
	cp $(SAMPDIR)/internat/fr/*.mo $(DISTDIR)/samples/internat/fr
	cp $(SAMPDIR)/internat/de/*.mo $(DISTDIR)/samples/internat/de
	cp $(SAMPDIR)/internat/ja/*.mo $(DISTDIR)/samples/internat/ja
	cp $(SAMPDIR)/internat/ru/*.mo $(DISTDIR)/samples/internat/ru
	cp $(SAMPDIR)/internat/fr/*.po $(DISTDIR)/samples/internat/fr
	cp $(SAMPDIR)/internat/de/*.po $(DISTDIR)/samples/internat/de
	cp $(SAMPDIR)/internat/ja/*.po $(DISTDIR)/samples/internat/ja
	cp $(SAMPDIR)/internat/ru/*.po $(DISTDIR)/samples/internat/ru

	mkdir $(DISTDIR)/samples/ipc
	cp $(SAMPDIR)/ipc/Makefile.in $(DISTDIR)/samples/ipc
	cp $(SAMPDIR)/ipc/makefile.unx $(DISTDIR)/samples/ipc
	cp $(SAMPDIR)/ipc/*.cpp $(DISTDIR)/samples/ipc
	cp $(SAMPDIR)/ipc/*.h $(DISTDIR)/samples/ipc
	cp $(SAMPDIR)/ipc/*.xpm $(DISTDIR)/samples/ipc

	mkdir $(DISTDIR)/samples/joytest
	cp $(SAMPDIR)/joytest/Makefile.in $(DISTDIR)/samples/joytest
	cp $(SAMPDIR)/joytest/makefile.unx $(DISTDIR)/samples/joytest
	cp $(SAMPDIR)/joytest/*.cpp $(DISTDIR)/samples/joytest
	cp $(SAMPDIR)/joytest/*.h $(DISTDIR)/samples/joytest
	cp $(SAMPDIR)/joytest/*.wav $(DISTDIR)/samples/joytest

	mkdir $(DISTDIR)/samples/keyboard
	cp $(SAMPDIR)/keyboard/Makefile.in $(DISTDIR)/samples/keyboard
	cp $(SAMPDIR)/keyboard/*.cpp $(DISTDIR)/samples/keyboard

	mkdir $(DISTDIR)/samples/layout
	cp $(SAMPDIR)/layout/Makefile.in $(DISTDIR)/samples/layout
	cp $(SAMPDIR)/layout/makefile.unx $(DISTDIR)/samples/layout
	cp $(SAMPDIR)/layout/*.cpp $(DISTDIR)/samples/layout
	cp $(SAMPDIR)/layout/*.h $(DISTDIR)/samples/layout

	mkdir $(DISTDIR)/samples/listctrl
	mkdir $(DISTDIR)/samples/listctrl/bitmaps
	cp $(SAMPDIR)/listctrl/Makefile.in $(DISTDIR)/samples/listctrl
	cp $(SAMPDIR)/listctrl/makefile.unx $(DISTDIR)/samples/listctrl
	cp $(SAMPDIR)/listctrl/*.cpp $(DISTDIR)/samples/listctrl
	cp $(SAMPDIR)/listctrl/*.h $(DISTDIR)/samples/listctrl
	cp $(SAMPDIR)/listctrl/*.xpm $(DISTDIR)/samples/listctrl
	cp $(SAMPDIR)/listctrl/bitmaps/*.xpm $(DISTDIR)/samples/listctrl/bitmaps

	mkdir $(DISTDIR)/samples/mdi
	mkdir $(DISTDIR)/samples/mdi/bitmaps
	cp $(SAMPDIR)/mdi/Makefile.in $(DISTDIR)/samples/mdi
	cp $(SAMPDIR)/mdi/makefile.unx $(DISTDIR)/samples/mdi
	cp $(SAMPDIR)/mdi/*.cpp $(DISTDIR)/samples/mdi
	cp $(SAMPDIR)/mdi/*.h $(DISTDIR)/samples/mdi
	cp $(SAMPDIR)/mdi/*.xpm $(DISTDIR)/samples/mdi
	cp $(SAMPDIR)/mdi/bitmaps/*.xpm $(DISTDIR)/samples/mdi/bitmaps

	mkdir $(DISTDIR)/samples/memcheck
	cp $(SAMPDIR)/memcheck/Makefile.in $(DISTDIR)/samples/memcheck
	cp $(SAMPDIR)/memcheck/makefile.unx $(DISTDIR)/samples/memcheck
	cp $(SAMPDIR)/memcheck/*.cpp $(DISTDIR)/samples/memcheck
	cp $(SAMPDIR)/memcheck/*.xpm $(DISTDIR)/samples/memcheck

	mkdir $(DISTDIR)/samples/menu
	cp $(SAMPDIR)/menu/Makefile.in $(DISTDIR)/samples/menu
	cp $(SAMPDIR)/menu/makefile.unx $(DISTDIR)/samples/menu
	cp $(SAMPDIR)/menu/*.cpp $(DISTDIR)/samples/menu
	cp $(SAMPDIR)/menu/*.xpm $(DISTDIR)/samples/menu

	mkdir $(DISTDIR)/samples/minifram
	mkdir $(DISTDIR)/samples/minifram/bitmaps
	cp $(SAMPDIR)/minifram/Makefile.in $(DISTDIR)/samples/minifram
	cp $(SAMPDIR)/minifram/makefile.unx $(DISTDIR)/samples/minifram
	cp $(SAMPDIR)/minifram/*.cpp $(DISTDIR)/samples/minifram
	cp $(SAMPDIR)/minifram/*.h $(DISTDIR)/samples/minifram
	cp $(SAMPDIR)/minifram/*.xpm $(DISTDIR)/samples/minifram
	cp $(SAMPDIR)/minifram/bitmaps/*.xpm $(DISTDIR)/samples/minifram/bitmaps

	mkdir $(DISTDIR)/samples/minimal
	cp $(SAMPDIR)/minimal/Makefile.in $(DISTDIR)/samples/minimal
	cp $(SAMPDIR)/minimal/makefile.unx $(DISTDIR)/samples/minimal
	cp $(SAMPDIR)/minimal/*.cpp $(DISTDIR)/samples/minimal
	cp $(SAMPDIR)/minimal/*.mms $(DISTDIR)/samples/minimal

	mkdir $(DISTDIR)/samples/mobile
	cp $(SAMPDIR)/mobile/Makefile.in $(DISTDIR)/samples/mobile
	mkdir $(DISTDIR)/samples/mobile/wxedit
	cp $(SAMPDIR)/mobile/wxedit/Makefile.in $(DISTDIR)/samples/mobile/wxedit
	cp $(SAMPDIR)/mobile/wxedit/*.cpp $(DISTDIR)/samples/mobile/wxedit
	cp $(SAMPDIR)/mobile/wxedit/*.h $(DISTDIR)/samples/mobile/wxedit
	mkdir $(DISTDIR)/samples/mobile/styles
	cp $(SAMPDIR)/mobile/styles/Makefile.in $(DISTDIR)/samples/mobile/styles
	cp $(SAMPDIR)/mobile/styles/*.cpp $(DISTDIR)/samples/mobile/styles
	cp $(SAMPDIR)/mobile/styles/*.h $(DISTDIR)/samples/mobile/styles
	cp $(SAMPDIR)/mobile/styles/*.jpg $(DISTDIR)/samples/mobile/styles
    
	mkdir $(DISTDIR)/samples/multimon
	cp $(SAMPDIR)/multimon/Makefile.in $(DISTDIR)/samples/multimon
	cp $(SAMPDIR)/multimon/makefile.unx $(DISTDIR)/samples/multimon
	cp $(SAMPDIR)/multimon/*.cpp $(DISTDIR)/samples/multimon

	mkdir $(DISTDIR)/samples/notebook
	cp $(SAMPDIR)/notebook/Makefile.in $(DISTDIR)/samples/notebook
	cp $(SAMPDIR)/notebook/makefile.unx $(DISTDIR)/samples/notebook
	cp $(SAMPDIR)/notebook/*.cpp $(DISTDIR)/samples/notebook
	cp $(SAMPDIR)/notebook/*.h $(DISTDIR)/samples/notebook

	mkdir $(DISTDIR)/samples/opengl
	cp $(SAMPDIR)/opengl/Makefile.in $(DISTDIR)/samples/opengl
	mkdir $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/Makefile.in $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/makefile.unx $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/*.cpp $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/*.c $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/*.h $(DISTDIR)/samples/opengl/penguin
	cp $(SAMPDIR)/opengl/penguin/*.lwo $(DISTDIR)/samples/opengl/penguin
	mkdir $(DISTDIR)/samples/opengl/cube
	cp $(SAMPDIR)/opengl/cube/Makefile.in $(DISTDIR)/samples/opengl/cube
	cp $(SAMPDIR)/opengl/cube/makefile.unx $(DISTDIR)/samples/opengl/cube
	cp $(SAMPDIR)/opengl/cube/*.cpp $(DISTDIR)/samples/opengl/cube
	cp $(SAMPDIR)/opengl/cube/*.h $(DISTDIR)/samples/opengl/cube
	mkdir $(DISTDIR)/samples/opengl/isosurf
	cp $(SAMPDIR)/opengl/isosurf/Makefile.in $(DISTDIR)/samples/opengl/isosurf
	cp $(SAMPDIR)/opengl/isosurf/makefile.unx $(DISTDIR)/samples/opengl/isosurf
	cp $(SAMPDIR)/opengl/isosurf/*.cpp $(DISTDIR)/samples/opengl/isosurf
	cp $(SAMPDIR)/opengl/isosurf/*.h $(DISTDIR)/samples/opengl/isosurf
	cp $(SAMPDIR)/opengl/isosurf/*.gz $(DISTDIR)/samples/opengl/isosurf

	mkdir $(DISTDIR)/samples/png
	cp $(SAMPDIR)/png/Makefile.in $(DISTDIR)/samples/png
	cp $(SAMPDIR)/png/makefile.unx $(DISTDIR)/samples/png
	cp $(SAMPDIR)/png/*.cpp $(DISTDIR)/samples/png
	cp $(SAMPDIR)/png/*.h $(DISTDIR)/samples/png
	cp $(SAMPDIR)/png/*.png $(DISTDIR)/samples/png

	mkdir $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/Makefile.in $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/makefile.unx $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/*.cpp $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/*.h $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/*.xpm $(DISTDIR)/samples/printing
	cp $(SAMPDIR)/printing/*.xbm $(DISTDIR)/samples/printing

#	mkdir $(DISTDIR)/samples/resource
#	cp $(SAMPDIR)/resource/Makefile.in $(DISTDIR)/samples/resource
#	cp $(SAMPDIR)/resource/makefile.unx $(DISTDIR)/samples/resource
#	cp $(SAMPDIR)/resource/*.cpp $(DISTDIR)/samples/resource
#	cp $(SAMPDIR)/resource/*.h $(DISTDIR)/samples/resource
#	cp $(SAMPDIR)/resource/*.wxr $(DISTDIR)/samples/resource

	mkdir $(DISTDIR)/samples/render
	cp $(SAMPDIR)/render/Makefile.in $(DISTDIR)/samples/render
	cp $(SAMPDIR)/render/makefile.unx $(DISTDIR)/samples/render
	cp $(SAMPDIR)/render/*.cpp $(DISTDIR)/samples/render

	mkdir $(DISTDIR)/samples/rotate
	cp $(SAMPDIR)/rotate/Makefile.in $(DISTDIR)/samples/rotate
	cp $(SAMPDIR)/rotate/makefile.unx $(DISTDIR)/samples/rotate
	cp $(SAMPDIR)/rotate/*.cpp $(DISTDIR)/samples/rotate
	cp $(SAMPDIR)/rotate/*.bmp $(DISTDIR)/samples/rotate

	mkdir $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/Makefile.in $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/makefile.unx $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/*.cpp $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/*.h $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/*.xpm $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/README $(DISTDIR)/samples/richedit
	cp $(SAMPDIR)/richedit/TODO $(DISTDIR)/samples/richedit

#	mkdir $(DISTDIR)/samples/proplist
#	cp $(SAMPDIR)/proplist/Makefile.in $(DISTDIR)/samples/proplist
#	cp $(SAMPDIR)/proplist/makefile.unx $(DISTDIR)/samples/proplist
#	cp $(SAMPDIR)/proplist/*.cpp $(DISTDIR)/samples/proplist
#	cp $(SAMPDIR)/proplist/*.h $(DISTDIR)/samples/proplist

	mkdir $(DISTDIR)/samples/propsize
	cp $(SAMPDIR)/propsize/Makefile.in $(DISTDIR)/samples/propsize
	cp $(SAMPDIR)/propsize/makefile.unx $(DISTDIR)/samples/propsize
	cp $(SAMPDIR)/propsize/*.cpp $(DISTDIR)/samples/propsize
	cp $(SAMPDIR)/propsize/*.xpm $(DISTDIR)/samples/propsize

	mkdir $(DISTDIR)/samples/sashtest
	cp $(SAMPDIR)/sashtest/Makefile.in $(DISTDIR)/samples/sashtest
	cp $(SAMPDIR)/sashtest/makefile.unx $(DISTDIR)/samples/sashtest
	cp $(SAMPDIR)/sashtest/*.cpp $(DISTDIR)/samples/sashtest
	cp $(SAMPDIR)/sashtest/*.h $(DISTDIR)/samples/sashtest

	mkdir $(DISTDIR)/samples/scroll
	cp $(SAMPDIR)/scroll/Makefile.in $(DISTDIR)/samples/scroll
	cp $(SAMPDIR)/scroll/makefile.unx $(DISTDIR)/samples/scroll
	cp $(SAMPDIR)/scroll/*.cpp $(DISTDIR)/samples/scroll

	mkdir $(DISTDIR)/samples/scrollsub
	cp $(SAMPDIR)/scrollsub/Makefile.in $(DISTDIR)/samples/scrollsub
	cp $(SAMPDIR)/scrollsub/makefile.unx $(DISTDIR)/samples/scrollsub
	cp $(SAMPDIR)/scrollsub/*.cpp $(DISTDIR)/samples/scrollsub

	mkdir $(DISTDIR)/samples/shaped
	cp $(SAMPDIR)/shaped/Makefile.in $(DISTDIR)/samples/shaped
	cp $(SAMPDIR)/shaped/makefile.unx $(DISTDIR)/samples/shaped
	cp $(SAMPDIR)/shaped/*.cpp $(DISTDIR)/samples/shaped
	cp $(SAMPDIR)/shaped/*.png $(DISTDIR)/samples/shaped

	mkdir $(DISTDIR)/samples/sockets
	cp $(SAMPDIR)/sockets/Makefile.in $(DISTDIR)/samples/sockets
	cp $(SAMPDIR)/sockets/makefile.unx $(DISTDIR)/samples/sockets
	cp $(SAMPDIR)/sockets/*.cpp $(DISTDIR)/samples/sockets
	cp $(SAMPDIR)/sockets/*.xpm $(DISTDIR)/samples/sockets

	mkdir $(DISTDIR)/samples/sound
	cp $(SAMPDIR)/sound/Makefile.in $(DISTDIR)/samples/sound
	cp $(SAMPDIR)/sound/*.cpp $(DISTDIR)/samples/sound
	cp $(SAMPDIR)/sound/*.wav $(DISTDIR)/samples/sound

	mkdir $(DISTDIR)/samples/splash
	cp $(SAMPDIR)/splash/Makefile.in $(DISTDIR)/samples/splash
	cp $(SAMPDIR)/splash/*.cpp $(DISTDIR)/samples/splash
	cp $(SAMPDIR)/splash/*.png $(DISTDIR)/samples/splash

	mkdir $(DISTDIR)/samples/splitter
	cp $(SAMPDIR)/splitter/Makefile.in $(DISTDIR)/samples/splitter
	cp $(SAMPDIR)/splitter/makefile.unx $(DISTDIR)/samples/splitter
	cp $(SAMPDIR)/splitter/*.cpp $(DISTDIR)/samples/splitter

	mkdir $(DISTDIR)/samples/statbar
	cp $(SAMPDIR)/statbar/Makefile.in $(DISTDIR)/samples/statbar
	cp $(SAMPDIR)/statbar/makefile.unx $(DISTDIR)/samples/statbar
	cp $(SAMPDIR)/statbar/*.cpp $(DISTDIR)/samples/statbar
	cp $(SAMPDIR)/statbar/*.xpm $(DISTDIR)/samples/statbar

	mkdir $(DISTDIR)/samples/text
	cp $(SAMPDIR)/text/Makefile.in $(DISTDIR)/samples/text
	cp $(SAMPDIR)/text/makefile.unx $(DISTDIR)/samples/text
	cp $(SAMPDIR)/text/*.cpp $(DISTDIR)/samples/text
	cp $(SAMPDIR)/text/*.xpm $(DISTDIR)/samples/text

	mkdir $(DISTDIR)/samples/thread
	cp $(SAMPDIR)/thread/Makefile.in $(DISTDIR)/samples/thread
	cp $(SAMPDIR)/thread/makefile.unx $(DISTDIR)/samples/thread
	cp $(SAMPDIR)/thread/*.cpp $(DISTDIR)/samples/thread

	mkdir $(DISTDIR)/samples/toolbar
	cp $(SAMPDIR)/toolbar/Makefile.in $(DISTDIR)/samples/toolbar
	cp $(SAMPDIR)/toolbar/makefile.unx $(DISTDIR)/samples/toolbar
	cp $(SAMPDIR)/toolbar/*.cpp $(DISTDIR)/samples/toolbar
	cp $(SAMPDIR)/toolbar/*.xpm $(DISTDIR)/samples/toolbar
	mkdir $(DISTDIR)/samples/toolbar/bitmaps
	cp $(SAMPDIR)/toolbar/bitmaps/*.xpm $(DISTDIR)/samples/toolbar/bitmaps

	mkdir $(DISTDIR)/samples/treectrl
	cp $(SAMPDIR)/treectrl/Makefile.in $(DISTDIR)/samples/treectrl
	cp $(SAMPDIR)/treectrl/makefile.unx $(DISTDIR)/samples/treectrl
	cp $(SAMPDIR)/treectrl/*.cpp $(DISTDIR)/samples/treectrl
	cp $(SAMPDIR)/treectrl/*.h $(DISTDIR)/samples/treectrl
	cp $(SAMPDIR)/treectrl/*.xpm $(DISTDIR)/samples/treectrl

#	mkdir $(DISTDIR)/samples/treelay
#	cp $(SAMPDIR)/treelay/Makefile.in $(DISTDIR)/samples/treelay
#	cp $(SAMPDIR)/treelay/makefile.unx $(DISTDIR)/samples/treelay
#	cp $(SAMPDIR)/treelay/*.cpp $(DISTDIR)/samples/treelay
#	cp $(SAMPDIR)/treelay/*.h $(DISTDIR)/samples/treelay

	mkdir $(DISTDIR)/samples/typetest
	cp $(SAMPDIR)/typetest/Makefile.in $(DISTDIR)/samples/typetest
	cp $(SAMPDIR)/typetest/makefile.unx $(DISTDIR)/samples/typetest
	cp $(SAMPDIR)/typetest/*.cpp $(DISTDIR)/samples/typetest
	cp $(SAMPDIR)/typetest/*.h $(DISTDIR)/samples/typetest
	cp $(SAMPDIR)/typetest/*.xpm $(DISTDIR)/samples/typetest

	mkdir $(DISTDIR)/samples/validate
	cp $(SAMPDIR)/validate/Makefile.in $(DISTDIR)/samples/validate
	cp $(SAMPDIR)/validate/makefile.unx $(DISTDIR)/samples/validate
	cp $(SAMPDIR)/validate/*.cpp $(DISTDIR)/samples/validate
	cp $(SAMPDIR)/validate/*.h $(DISTDIR)/samples/validate
	cp $(SAMPDIR)/validate/*.xpm $(DISTDIR)/samples/validate

	mkdir $(DISTDIR)/samples/wizard
	cp $(SAMPDIR)/wizard/Makefile.in $(DISTDIR)/samples/wizard
	cp $(SAMPDIR)/wizard/makefile.unx $(DISTDIR)/samples/wizard
	cp $(SAMPDIR)/wizard/*.cpp $(DISTDIR)/samples/wizard
	cp $(SAMPDIR)/wizard/*.xpm $(DISTDIR)/samples/wizard

	mkdir $(DISTDIR)/samples/widgets
	mkdir $(DISTDIR)/samples/widgets/icons
	cp $(SAMPDIR)/widgets/Makefile.in $(DISTDIR)/samples/widgets
	cp $(SAMPDIR)/widgets/*.cpp $(DISTDIR)/samples/widgets
	cp $(SAMPDIR)/widgets/*.h $(DISTDIR)/samples/widgets
	cp $(SAMPDIR)/widgets/*.rc $(DISTDIR)/samples/widgets
	cp $(SAMPDIR)/widgets/icons/*.xpm $(DISTDIR)/samples/widgets/icons

UTILS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/utils
	cp $(UTILSDIR)/Makefile.in $(DISTDIR)/utils

	mkdir $(DISTDIR)/utils/tex2rtf
	mkdir $(DISTDIR)/utils/tex2rtf/src
	cp $(UTILSDIR)/tex2rtf/*.in $(DISTDIR)/utils/tex2rtf
	cp $(UTILSDIR)/tex2rtf/src/*.h $(DISTDIR)/utils/tex2rtf/src
	cp $(UTILSDIR)/tex2rtf/src/*.in $(DISTDIR)/utils/tex2rtf/src
	cp $(UTILSDIR)/tex2rtf/src/*.cpp $(DISTDIR)/utils/tex2rtf/src
	cp $(UTILSDIR)/tex2rtf/src/tex2rtf.* $(DISTDIR)/utils/tex2rtf/src

	mkdir $(DISTDIR)/utils/emulator
	mkdir $(DISTDIR)/utils/emulator/src
	mkdir $(DISTDIR)/utils/emulator/docs
	cp $(UTILSDIR)/emulator/*.in $(DISTDIR)/utils/emulator
	cp $(UTILSDIR)/emulator/src/*.h $(DISTDIR)/utils/emulator/src
	cp $(UTILSDIR)/emulator/src/*.in $(DISTDIR)/utils/emulator/src
	cp $(UTILSDIR)/emulator/src/*.cpp $(DISTDIR)/utils/emulator/src
	cp $(UTILSDIR)/emulator/src/*.jpg $(DISTDIR)/utils/emulator/src
	cp $(UTILSDIR)/emulator/src/*.wxe $(DISTDIR)/utils/emulator/src
	cp $(UTILSDIR)/emulator/docs/*.txt $(DISTDIR)/utils/emulator/docs
	cp $(UTILSDIR)/emulator/docs/*.jpg $(DISTDIR)/utils/emulator/docs

	mkdir $(DISTDIR)/utils/hhp2cached
	cp $(UTILSDIR)/hhp2cached/Makefile.in $(DISTDIR)/utils/hhp2cached
	cp $(UTILSDIR)/hhp2cached/*.cpp $(DISTDIR)/utils/hhp2cached
	cp $(UTILSDIR)/hhp2cached/*.rc $(DISTDIR)/utils/hhp2cached

	mkdir $(DISTDIR)/utils/HelpGen
	mkdir $(DISTDIR)/utils/HelpGen/src
	cp $(UTILSDIR)/HelpGen/Makefile.in $(DISTDIR)/utils/HelpGen
	cp $(UTILSDIR)/HelpGen/src/Makefile.in $(DISTDIR)/utils/HelpGen/src
	cp $(UTILSDIR)/HelpGen/src/*.h $(DISTDIR)/utils/HelpGen/src
	cp $(UTILSDIR)/HelpGen/src/*.cpp $(DISTDIR)/utils/HelpGen/src

	mkdir $(DISTDIR)/utils/helpview
	mkdir $(DISTDIR)/utils/helpview/src
	mkdir $(DISTDIR)/utils/helpview/src/bitmaps
	cp $(UTILSDIR)/helpview/Makefile.in $(DISTDIR)/utils/helpview
	cp $(UTILSDIR)/helpview/src/*.h $(DISTDIR)/utils/helpview/src
	cp $(UTILSDIR)/helpview/src/*.cpp $(DISTDIR)/utils/helpview/src
	cp $(UTILSDIR)/helpview/src/Makefile.in $(DISTDIR)/utils/helpview/src
	cp $(UTILSDIR)/helpview/src/test.zip $(DISTDIR)/utils/helpview/src
	cp $(UTILSDIR)/helpview/src/bitmaps/*.xpm $(DISTDIR)/utils/helpview/src/bitmaps

	mkdir $(DISTDIR)/utils/wxrc
	cp $(UTILSDIR)/wxrc/Makefile.in $(DISTDIR)/utils/wxrc
	cp $(UTILSDIR)/wxrc/*.cpp $(DISTDIR)/utils/wxrc
	cp $(UTILSDIR)/wxrc/*.rc $(DISTDIR)/utils/wxrc

MISC_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/misc
	mkdir $(DISTDIR)/misc/afm
	cp $(MISCDIR)/afm/*.afm  $(DISTDIR)/misc/afm
	mkdir $(DISTDIR)/misc/gs_afm
	cp $(MISCDIR)/gs_afm/*.afm  $(DISTDIR)/misc/gs_afm

INTL_DIST:
	mkdir $(DISTDIR)/locale
	cp $(INTLDIR)/Makefile $(DISTDIR)/locale
	cp $(INTLDIR)/*.po $(DISTDIR)/locale
	-cp $(INTLDIR)/*.mo $(DISTDIR)/locale

MANUAL_DIST:
	mkdir $(DISTDIR)/docs
	mkdir $(DISTDIR)/docs/latex
	mkdir $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.tex $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.inc $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.gif $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.ini $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.bib $(DISTDIR)/docs/latex/wx
	cp $(DOCDIR)/latex/wx/*.sty $(DISTDIR)/docs/latex/wx


# Copy all the files from wxPython needed for the Debian source package, 
# and then remove some that are not needed.
PYTHON_DIST:
	for dir in `grep -v '#' $(WXDIR)/wxPython/distrib/DIRLIST`; do \
		echo "Copying dir: $$dir..."; \
		mkdir $(DISTDIR)/$$dir; \
		cp $(WXDIR)/$$dir/* $(DISTDIR)/$$dir > /dev/null 2>&1; \
	done; \
	\
	find $(DISTDIR)/wxPython -name "*~"           > RM_FILES; \
	find $(DISTDIR)/wxPython -name "*.pyc"       >> RM_FILES; \
	find $(DISTDIR)/wxPython -name "*.bat"       >> RM_FILES; \
	find $(DISTDIR)/wxPython -name "core"        >> RM_FILES; \
	find $(DISTDIR)/wxPython -name "core.[0-9]*" >> RM_FILES; \
	find $(DISTDIR)/wxPython -name "*.orig"      >> RM_FILES; \
	find $(DISTDIR)/wxPython -name "*.rej"       >> RM_FILES; \
	for f in `cat RM_FILES`; do rm $$f; done; \
	rm RM_FILES


distrib_clean:
	$(RM) -r _dist_dir

# VZ: the -only targets allow to do "make dist bzip-dist-only" without copying
#     the files twice
dist-only:
	@echo "*** Creating wxWidgets distribution in $(DISTDIR)..."
	@cd _dist_dir && tar ch $(DISTDIRNAME) | gzip -f9 > ../$(WXARCHIVE);
	@if test "$(USE_GUI)" = 1; then \
	cd $(DISTDIR); \
	mv samples wxSamples-$(WX_VERSION); \
	echo "*** Creating wxSamples archive..."; \
	tar ch wxSamples-$(WX_VERSION) | gzip -f9 > ../../$(WXSAMPLES); \
	mv wxSamples-$(WX_VERSION) samples; \
	mv demos wxDemos-$(WX_VERSION); \
	echo "*** Creating wxDemos archive..."; \
	tar ch wxDemos-$(WX_VERSION) | gzip -f9 > ../../$(WXDEMOS); \
	mv wxDemos-$(WX_VERSION) demos; \
	fi

dist: @GUIDIST@
	@echo "*** Creating wxWidgets distribution in $(DISTDIR)..."
	@# now prune away a lot of the crap included by using cp -R
	@# in other dist targets.
	find $(DISTDIR) \( -name "CVS" -o -name ".cvsignore" -o -name "*.dsp" -o -name "*.dsw" -o -name "*.hh*" -o \
			\( -name "makefile.*" -a ! -name "makefile.unx" \) \) \
			-print0 | xargs -0 rm -rf
	@cd _dist_dir && tar ch $(DISTDIRNAME) | gzip -f9 > ../$(WXARCHIVE);
	@if test "$(USE_GUI)" = 1; then \
	cd $(DISTDIR); \
	mv samples wxSamples-$(WX_VERSION); \
	echo "*** Creating wxSamples archive..."; \
	tar ch wxSamples-$(WX_VERSION) | gzip -f9 > ../../$(WXSAMPLES); \
	mv wxSamples-$(WX_VERSION) samples; \
	mv demos wxDemos-$(WX_VERSION); \
	echo "*** Creating wxDemos archive..."; \
	tar ch wxDemos-$(WX_VERSION) | gzip -f9 > ../../$(WXDEMOS); \
	mv wxDemos-$(WX_VERSION) demos; \
	fi

bzip-dist-only:
	@echo "*** Creating wxWidgets distribution in $(DISTDIR)..."
	@cd _dist_dir && tar ch $(DISTDIRNAME) | bzip2 -f9 > ../$(WXARCHIVE_BZIP);
	@if test "$(USE_GUI)" = 1; then \
	cd $(DISTDIR); \
	mv samples wxSamples-${WX_VERSION}; \
	echo "*** Creating wxSamples archive..."; \
	tar ch wxSamples-${WX_VERSION} | bzip2 -f9 > ../../$(WXSAMPLES_BZIP); \
	mv wxSamples-${WX_VERSION} samples; \
	mv demos wxDemos-${WX_VERSION}; \
	echo "*** Creating wxDemos archive..."; \
	tar ch wxDemos-${WX_VERSION} | bzip2 -f9 > ../../$(WXDEMOS_BZIP); \
	mv wxDemos-${WX_VERSION} demos; \
	fi

bzip-dist: @GUIDIST@
	@echo "*** Creating wxWidgets distribution in $(DISTDIR)..."
	@cd _dist_dir && tar ch $(DISTDIRNAME) | bzip2 -f9 > ../$(WXARCHIVE_BZIP)
	@if test "$(USE_GUI)" = 1; then \
	cd $(DISTDIR); \
	mv samples wxSamples; \
	tar ch wxSamples | bzip2 -f9 > ../../$(WXSAMPLES_BZIP); \
	mv wxSamples samples; \
	mv demos wxDemos; \
	tar ch wxDemos | bzip2 -f9 > ../../$(WXDEMOS_BZIP); \
	mv wxDemos demos; \
	fi

# RR: Copy text and binary data separatly
win-dist: MSW_ZIP_TEXT_DIST
	@echo "*** Creating wxWidgets ZIP distribution in $(DISTDIR)..."
	@cd _dist_dir && mv $(DISTDIRNAME) wxMSW
	@cd _dist_dir && zip -r -l  ../$(WXARCHIVE_ZIP) *
	cp $(INCDIR)/wx/msw/*.cur _dist_dir/wxMSW/include/wx/msw
	cp $(INCDIR)/wx/msw/*.ico _dist_dir/wxMSW/include/wx/msw
	cp $(INCDIR)/wx/msw/*.bmp _dist_dir/wxMSW/include/wx/msw
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.cur
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.ico
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.bmp

debian-dist: DEBIAN_SOURCE_DIR = $(WXDIR)/../wxwidgets@WX_RELEASE@@WX_FLAVOUR@-@WX_SUBVERSION@
debian-dist: debian-native-dist debian-msw-dirs MSW_DIST
	mkdir $(DISTDIR)/debian
	-cp $(WXDIR)/debian/* $(DISTDIR)/debian
	cp $(DOCDIR)/licence.txt $(DISTDIR)/docs
	cp $(DOCDIR)/licendoc.txt $(DISTDIR)/docs
	cp $(DOCDIR)/preamble.txt $(DISTDIR)/docs
	rm -f $(DISTDIR)/*.spec

	@# now prune away a lot of the crap included by using cp -R
	@# in other dist targets.  Ugly and hardly portable but it
	@# will run on any Debian box and that's enough for now.

	rm -rf $(DISTDIR)/contrib/build
	find $(DISTDIR) \( -name "CVS" -o -name ".cvsignore" -o -name "*.dsp"    \
			   -o -name "*.dsw" -o -name "*.hh*" -o -name "*.mms"    \
			   -o -name "*.mcp" -o -name "*M*.xml" -o -name "*.r"    \
			   -o -name "*.bkl" -o -name "*.pro" -o -name "*.def"    \
			   -o -name "*.vpj" -o -name "*.sc"                      \
			   -o \( -name "makefile.*" -a ! -name "makefile.unx" \) \
			\) -print0 | xargs -0 rm -rf

	rm -rf $(DISTDIR)/wxPython/SWIG
	rm -rf $(DISTDIR)/wxPython/distrib
	rm -rf $(DISTDIR)/wxPython/distutils
	rm -rf $(DISTDIR)/wxPython/samples
	rm -rf $(DISTDIR)/wxPython/contrib/iewin
	find $(DISTDIR)/wxPython \( -name "mac" -o -name "msw" \) -print0 | xargs -0 rm -rf

	rm -rf $(DEBIAN_SOURCE_DIR)
	mv $(DISTDIR) $(DEBIAN_SOURCE_DIR)


debian-native-dist: @GUIDIST@ UNIV_DIST MANUAL_DIST PYTHON_DIST

debian-msw-dirs:
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/src/msw


RPMTOP=_dist_dir/_rpm_top

rpm: bzip-dist
	@echo "*** Building RPMs ***"
	-mkdir $(RPMTOP)
	-mkdir $(RPMTOP)/SOURCES
	-mkdir $(RPMTOP)/SPECS
	-mkdir $(RPMTOP)/BUILD
	-mkdir $(RPMTOP)/RPMS
	-mkdir $(RPMTOP)/SRPMS
	cp -f $(WXARCHIVE_BZIP) $(RPMTOP)/SOURCES
	rpm -ba --define "_topdir `pwd`/$(RPMTOP)" $(WXDIR)/wx$(TOOLKIT).spec
	mv -f `find $(RPMTOP) -name "wx$(TOOLKIT)*.rpm"` .
