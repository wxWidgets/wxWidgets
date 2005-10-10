#
# "make dist" target implementation:
#


############################# Dirs #################################

WXDIR = $(shell echo $(top_srcdir) | grep '^/' || echo `pwd`/$(top_srcdir) )

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
MSDOSDIR = $(WXDIR)/src/msdos
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
	ln -sf $(WXDIR)/configure.in $(DISTDIR)
	ln -sf $(WXDIR)/configure $(DISTDIR)
	ln -sf $(WXDIR)/autoconf_inc.m4 $(DISTDIR)
	ln -sf $(WXDIR)/wxwin.m4 $(DISTDIR)
	ln -sf $(WXDIR)/aclocal.m4 $(DISTDIR)
	ln -sf $(WXDIR)/config.sub $(DISTDIR)
	ln -sf $(WXDIR)/config.guess $(DISTDIR)
	ln -sf $(WXDIR)/install-sh $(DISTDIR)
	ln -sf $(WXDIR)/mkinstalldirs $(DISTDIR)
	ln -sf $(WXDIR)/wx-config.in $(DISTDIR)
	ln -sf $(WXDIR)/wx-config-inplace.in $(DISTDIR)
	ln -sf $(WXDIR)/version-script.in $(DISTDIR)
	ln -sf $(WXDIR)/setup.h.in $(DISTDIR)
	ln -sf $(WXDIR)/setup.h_vms $(DISTDIR)
	ln -sf $(WXDIR)/descrip.mms $(DISTDIR)
	ln -sf $(WXDIR)/Makefile.in $(DISTDIR)
	ln -sf $(WXDIR)/wxBase.spec $(DISTDIR)
	ln -sf $(DOCDIR)/lgpl.txt $(DISTDIR)/COPYING.LIB
	ln -sf $(DOCDIR)/licence.txt $(DISTDIR)/LICENCE.txt
	ln -sf $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	mkdir $(DISTDIR)/lib
	ln -sf $(WXDIR)/lib/vms.opt $(DISTDIR)/lib
	ln -sf $(WXDIR)/lib/vms_gtk.opt $(DISTDIR)/lib
	mkdir $(DISTDIR)/src
	# temp hack for common/execcmn.cpp: it's not supported by tmake
	# yet (it's a header-like file but in src/common directory and it
	# shouldn't be distributed...)
	mkdir $(DISTDIR)/src/common
	ln -sf $(SRCDIR)/common/execcmn.cpp $(DISTDIR)/src/common
	mkdir $(DISTDIR)/src/xml
	ln -sf $(SRCDIR)/xml/*.cpp $(DISTDIR)/src/xml
	mkdir $(DISTDIR)/src/zlib
	ln -sf $(ZLIBDIR)/*.h $(DISTDIR)/src/zlib
	ln -sf $(ZLIBDIR)/*.c $(DISTDIR)/src/zlib
	ln -sf $(ZLIBDIR)/README $(DISTDIR)/src/zlib
	#ln -sf $(ZLIBDIR)/*.mms $(DISTDIR)/src/zlib
	mkdir $(DISTDIR)/src/regex
	ln -sf $(REGEXDIR)/*.h $(DISTDIR)/src/regex
	ln -sf $(REGEXDIR)/*.c $(DISTDIR)/src/regex
	ln -sf $(REGEXDIR)/COPYRIGHT $(DISTDIR)/src/regex
	ln -sf $(REGEXDIR)/README $(DISTDIR)/src/regex
	mkdir $(DISTDIR)/src/expat
	lndir $(EXPATDIR) $(DISTDIR)/src/expat
	#(cd $(DISTDIR)/src/expat ; rm -rf `find -name CVS`)
	mkdir $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/*.h $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/*.c $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/*.ci $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/*.exp $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/README $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/NEWS $(DISTDIR)/src/iodbc
	ln -sf $(ODBCDIR)/Changes.log $(DISTDIR)/src/iodbc
	# copy some files from include/ that are not installed:
	mkdir $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx
	ln -sf $(INCDIR)/wx/fmappriv.h $(DISTDIR)/include/wx
	# copy wxpresets
	mkdir $(DISTDIR)/build
	mkdir $(DISTDIR)/build/bakefiles
	mkdir $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/presets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/sample
	ln -sf $(WXDIR)/build/bakefiles/wxpresets/presets/*.bkl $(DISTDIR)/build/bakefiles/wxpresets/presets
	ln -sf $(WXDIR)/build/bakefiles/wxpresets/sample/minimal* $(DISTDIR)/build/bakefiles/wxpresets/sample
	ln -sf $(WXDIR)/build/bakefiles/wxpresets/sample/config* $(DISTDIR)/build/bakefiles/wxpresets/sample
	ln -sf $(WXDIR)/build/bakefiles/wxpresets/*.txt $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/aclocal
	ln -sf $(WXDIR)/build/aclocal/*.m4 $(DISTDIR)/build/aclocal

# this target is the common part of distribution script for all GUI toolkits,
# but is not used when building wxBase distribution
ALL_GUI_DIST: ALL_DIST
	ln -sf $(DOCDIR)/readme.txt $(DISTDIR)/README.txt
	ln -sf $(DOCDIR)/$(TOOLKITDIR)/install.txt $(DISTDIR)/INSTALL.txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/changes.txt ; then \
	  ln -sf $(DOCDIR)/$(TOOLKITDIR)/changes.txt $(DISTDIR)/CHANGES-$(TOOLKIT).txt ; fi
	ln -sf $(DOCDIR)/$(TOOLKITDIR)/readme.txt $(DISTDIR)/README-$(TOOLKIT).txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/todo.txt ; then \
	  ln -sf $(DOCDIR)/$(TOOLKITDIR)/todo.txt $(DISTDIR)/TODO.txt ; fi
	mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)
	mkdir $(DISTDIR)/include/wx/generic
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/xrc
	ln -sf $(INCDIR)/wx/*.h $(DISTDIR)/include/wx
	ln -sf $(INCDIR)/wx/*.cpp $(DISTDIR)/include/wx
	ln -sf $(INCDIR)/wx/generic/*.h $(DISTDIR)/include/wx/generic
	ln -sf $(INCDIR)/wx/html/*.h $(DISTDIR)/include/wx/html
	ln -sf $(INCDIR)/wx/unix/*.h $(DISTDIR)/include/wx/unix
	ln -sf $(INCDIR)/wx/xml/*.h $(DISTDIR)/include/wx/xml
	ln -sf $(INCDIR)/wx/xrc/*.h $(DISTDIR)/include/wx/xrc
	ln -sf $(INCDIR)/wx/protocol/*.h $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/art
	mkdir $(DISTDIR)/art/gtk
	mkdir $(DISTDIR)/art/motif
	ln -sf $(WXDIR)/art/*.xpm $(DISTDIR)/art
	ln -sf $(WXDIR)/art/gtk/*.xpm $(DISTDIR)/art/gtk
	ln -sf $(WXDIR)/art/motif/*.xpm $(DISTDIR)/art/motif
	mkdir $(DISTDIR)/src/generic
	mkdir $(DISTDIR)/src/html
	mkdir $(DISTDIR)/src/$(TOOLKITDIR)
	mkdir $(DISTDIR)/src/png
	mkdir $(DISTDIR)/src/jpeg
	mkdir $(DISTDIR)/src/tiff
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/xrc
	ln -sf $(SRCDIR)/xrc/*.cpp $(DISTDIR)/src/xrc
	ln -sf $(COMMDIR)/*.cpp $(DISTDIR)/src/common
	ln -sf $(COMMDIR)/*.c $(DISTDIR)/src/common
	ln -sf $(COMMDIR)/*.inc $(DISTDIR)/src/common
	ln -sf $(COMMDIR)/*.mms $(DISTDIR)/src/common
	ln -sf $(UNIXDIR)/*.cpp $(DISTDIR)/src/unix
	ln -sf $(UNIXDIR)/*.mms $(DISTDIR)/src/unix
	ln -sf $(GENDIR)/*.cpp $(DISTDIR)/src/generic
	ln -sf $(GENDIR)/*.mms $(DISTDIR)/src/generic
	ln -sf $(HTMLDIR)/*.cpp $(DISTDIR)/src/html
	ln -sf $(PNGDIR)/*.h $(DISTDIR)/src/png
	ln -sf $(PNGDIR)/*.c $(DISTDIR)/src/png
	ln -sf $(PNGDIR)/README $(DISTDIR)/src/png
	ln -sf $(JPEGDIR)/*.h $(DISTDIR)/src/jpeg
	ln -sf $(JPEGDIR)/*.c $(DISTDIR)/src/jpeg
	ln -sf $(JPEGDIR)/README $(DISTDIR)/src/jpeg
	ln -sf $(TIFFDIR)/*.h $(DISTDIR)/src/tiff
	ln -sf $(TIFFDIR)/*.c $(DISTDIR)/src/tiff
	ln -sf $(TIFFDIR)/README $(DISTDIR)/src/tiff

BASE_DIST: ALL_DIST
	# make --disable-gui the default
	rm $(DISTDIR)/configure.in
	sed 's/DEFAULT_wxUSE_GUI=yes/DEFAULT_wxUSE_GUI=no/' \
		$(WXDIR)/configure.in > $(DISTDIR)/configure.in
	rm $(DISTDIR)/configure
	sed 's/DEFAULT_wxUSE_GUI=yes/DEFAULT_wxUSE_GUI=no/' \
		$(WXDIR)/configure > $(DISTDIR)/configure
	chmod +x $(DISTDIR)/configure
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/msdos
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/msdos
	mkdir $(DISTDIR)/src/msw
	ln -sf $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	ln -sf $(DOCDIR)/base/readme.txt $(DISTDIR)/README.txt
	ln -sf $(WXDIR)/src/common/*.inc $(DISTDIR)/src/common
	ln -sf $(WXDIR)/src/common/base.rc $(DISTDIR)/src/common
	list='$(ALL_BASE_HEADERS)'; for p in $$list; do \
	  ln -sf $(WXDIR)/include/$$p $(DISTDIR)/include/$$p; \
	done
	list='$(ALL_BASE_SOURCES)'; for p in $$list; do \
	  ln -sf $(WXDIR)/$$p $(DISTDIR)/$$p; \
	done

	mkdir $(DISTDIR)/samples
	ln -sf $(SAMPDIR)/Makefile.in $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/console.cpp $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/console.dsp $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

GTK_DIST: ALL_GUI_DIST
	ln -sf $(WXDIR)/wxGTK.spec $(DISTDIR)
	ln -sf $(INCDIR)/wx/gtk/*.h $(DISTDIR)/include/wx/gtk
	ln -sf $(GTKDIR)/*.h $(DISTDIR)/src/gtk
	ln -sf $(GTKDIR)/*.cpp $(DISTDIR)/src/gtk
	ln -sf $(GTKDIR)/*.c $(DISTDIR)/src/gtk
	ln -sf $(GTKDIR)/*.xbm $(DISTDIR)/src/gtk
	ln -sf $(GTKDIR)/*.mms $(DISTDIR)/src/gtk

	mkdir $(DISTDIR)/include/wx/gtk/gnome
	mkdir $(DISTDIR)/src/gtk/gnome
	ln -sf $(INCDIR)/wx/gtk/gnome/*.h $(DISTDIR)/include/wx/gtk/gnome
	ln -sf $(GTKDIR)/gnome/*.cpp $(DISTDIR)/src/gtk/gnome

	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	ln -sf $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	ln -sf $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation

	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

X11_DIST: ALL_GUI_DIST UNIV_DIST
	ln -sf $(WXDIR)/wxX11.spec $(DISTDIR)
	ln -sf $(INCDIR)/wx/x11/*.h $(DISTDIR)/include/wx/x11
	ln -sf $(X11DIR)/*.cpp $(DISTDIR)/src/x11
	ln -sf $(X11DIR)/*.c $(DISTDIR)/src/x11
	ln -sf $(X11DIR)/*.xbm $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	ln -sf $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	ln -sf $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

MOTIF_DIST: ALL_GUI_DIST
	ln -sf $(WXDIR)/wxMotif.spec $(DISTDIR)
	ln -sf $(INCDIR)/wx/motif/*.h $(DISTDIR)/include/wx/motif
	ln -sf $(MOTIFDIR)/*.cpp $(DISTDIR)/src/motif
	ln -sf $(MOTIFDIR)/*.xbm $(DISTDIR)/src/motif
	mkdir $(DISTDIR)/src/motif/xmcombo
	ln -sf $(MOTIFDIR)/xmcombo/*.c $(DISTDIR)/src/motif/xmcombo
	ln -sf $(MOTIFDIR)/xmcombo/*.h $(DISTDIR)/src/motif/xmcombo
	ln -sf $(MOTIFDIR)/xmcombo/copying.txt $(DISTDIR)/src/motif/xmcombo
	mkdir $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/include/wx/x11
	ln -sf $(X11DIR)/pen.cpp $(X11DIR)/brush.cpp $(X11DIR)/utilsx.cpp \
		$(X11DIR)/bitmap.cpp $(X11DIR)/glcanvas.cpp $(X11DIR)/region.cpp \
		$(DISTDIR)/src/x11
	ln -sf $(X11INC)/pen.h $(X11INC)/brush.h $(X11INC)/privx.h \
		$(X11INC)/bitmap.h $(X11INC)/glcanvas.h $(X11INC)/private.h $(X11INC)/region.h \
		$(DISTDIR)/include/wx/x11
	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

MACX_DIST: ALL_GUI_DIST
	ln -sf $(INCDIR)/*.* $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx/mac/carbon
	mkdir $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/carbon/private
	ln -sf $(INCDIR)/wx/mac/*.h $(DISTDIR)/include/wx/mac
	ln -sf $(INCDIR)/wx/mac/carbon/*.h $(DISTDIR)/include/wx/mac/carbon
	ln -sf $(INCDIR)/wx/mac/carbon/private/*.h $(DISTDIR)/include/wx/mac/carbon/private
	ln -sf $(INCDIR)/wx/mac/private/*.h $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	ln -sf $(INCDIR)/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/corefoundation
	ln -sf $(MACDIR)/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/src/html/htmlctrl
	mkdir $(DISTDIR)/src/html/htmlctrl/webkit
	ln -sf $(WXDIR)/src/html/htmlctrl/webkit/*.mm $(DISTDIR)/src/html/htmlctrl/webkit
	mkdir $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/*.cpp $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/*.mm $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/*.icns $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/*.h $(DISTDIR)/src/mac/carbon
	ln -sf $(MACDIR)/carbon/*.r $(DISTDIR)/src/mac/carbon
	mkdir $(DISTDIR)/src/mac/carbon/morefile
	ln -sf $(MACDIR)/carbon/morefile/*.h $(DISTDIR)/src/mac/carbon/morefile
	ln -sf $(MACDIR)/carbon/morefile/*.c $(DISTDIR)/src/mac/carbon/morefile
	mkdir $(DISTDIR)/src/mac/carbon/morefilex
	ln -sf $(MACDIR)/carbon/morefilex/*.h $(DISTDIR)/src/mac/carbon/morefilex
	ln -sf $(MACDIR)/carbon/morefilex/*.c $(DISTDIR)/src/mac/carbon/morefilex
	ln -sf $(MACDIR)/carbon/morefilex/*.cpp $(DISTDIR)/src/mac/carbon/morefilex
	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

# TODO: Distribute some files
COCOA_DIST: ALL_GUI_DIST
	ln -sf $(COCOADIR)/*.mm $(DISTDIR)/src/cocoa

MSW_DIST: ALL_GUI_DIST
	ln -sf $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	ln -sf $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.cur $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.ico $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.bmp $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/wx.manifest $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	ln -sf $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	ln -sf $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	ln -sf $(MSWDIR)/*.c $(DISTDIR)/src/msw
	ln -sf $(MSWDIR)/*.rc $(DISTDIR)/src/msw
	ln -sf $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole

MSW_ZIP_TEXT_DIST: ALL_GUI_DIST
	ln -sf $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	ln -sf $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/wx.manifest $(DISTDIR)/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	ln -sf $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	ln -sf $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	ln -sf $(MSWDIR)/*.c $(DISTDIR)/src/msw
	ln -sf $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	ln -sf $(MSWDIR)/wince/*.* $(DISTDIR)/src/msw/wince
	ln -sf $(SRCDIR)/*.??? $(DISTDIR)/src
	ln -sf $(SRCDIR)/*.?? $(DISTDIR)/src
	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

UNIV_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/include/wx/univ
	mkdir $(DISTDIR)/src/univ
	mkdir $(DISTDIR)/src/univ/themes
	ln -sf $(INCDIR)/wx/univ/*.h $(DISTDIR)/include/wx/univ
	ln -sf $(INCDIR)/wx/univ/setup0.h $(DISTDIR)/include/wx/univ/setup.h
	ln -sf $(SRCDIR)/univ/*.cpp $(DISTDIR)/src/univ
	ln -sf $(SRCDIR)/univ/themes/*.cpp $(DISTDIR)/src/univ/themes

MGL_DIST: UNIV_DIST
	ln -sf $(WXDIR)/wxMGL.spec $(DISTDIR)
	ln -sf $(INCDIR)/wx/mgl/*.h $(DISTDIR)/include/wx/mgl
	mkdir $(DISTDIR)/include/wx/msdos
	ln -sf $(INCDIR)/wx/msdos/*.h $(DISTDIR)/include/wx/msdos
	ln -sf $(SRCDIR)/mgl/make* $(DISTDIR)/src/mgl
	ln -sf $(SRCDIR)/mgl/*.cpp $(DISTDIR)/src/mgl
	mkdir $(DISTDIR)/src/msdos
	ln -sf $(SRCDIR)/msdos/*.cpp $(DISTDIR)/src/msdos
	mkdir $(DISTDIR)/contrib
	lndir $(WXDIR)/contrib $(DISTDIR)/contrib

DEMOS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/demos
	ln -sf $(DEMODIR)/Makefile.in $(DISTDIR)/demos

	mkdir $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/Makefile.in $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/makefile.unx $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/*.cpp $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/*.h $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/*.xpm $(DISTDIR)/demos/bombs
	ln -sf $(DEMODIR)/bombs/readme.txt $(DISTDIR)/demos/bombs

	mkdir $(DISTDIR)/demos/dbbrowse
	lndir $(DEMODIR)/dbbrowse $(DISTDIR)/demos/dbbrowse

	mkdir $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/Makefile.in $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/makefile.unx $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/*.h $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/*.cpp $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/*.xpm $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/*.xbm $(DISTDIR)/demos/forty
	ln -sf $(DEMODIR)/forty/*.htm $(DISTDIR)/demos/forty

	mkdir $(DISTDIR)/demos/life
	mkdir $(DISTDIR)/demos/life/bitmaps
	ln -sf $(DEMODIR)/life/Makefile.in $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/makefile.unx $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/*.cpp $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/*.h $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/*.xpm $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/*.inc $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/*.lif $(DISTDIR)/demos/life
	ln -sf $(DEMODIR)/life/bitmaps/*.xpm $(DISTDIR)/demos/life/bitmaps

	mkdir $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/Makefile.in $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/makefile.unx $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/*.h $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/*.cpp $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/*.xpm $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/*.dat $(DISTDIR)/demos/poem
	ln -sf $(DEMODIR)/poem/*.txt $(DISTDIR)/demos/poem

	mkdir $(DISTDIR)/demos/fractal
	ln -sf $(DEMODIR)/fractal/Makefile.in $(DISTDIR)/demos/fractal
	ln -sf $(DEMODIR)/fractal/makefile.unx $(DISTDIR)/demos/fractal
	ln -sf $(DEMODIR)/fractal/*.cpp $(DISTDIR)/demos/fractal

SAMPLES_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/samples
	ln -sf $(SAMPDIR)/Makefile.in $(DISTDIR)/samples
	ln -sf $(SAMPDIR)/sample.* $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/artprov
	ln -sf $(SAMPDIR)/artprov/Makefile.in $(DISTDIR)/samples/artprov
	ln -sf $(SAMPDIR)/artprov/makefile.unx $(DISTDIR)/samples/artprov
	ln -sf $(SAMPDIR)/artprov/*.cpp $(DISTDIR)/samples/artprov
	ln -sf $(SAMPDIR)/artprov/*.h $(DISTDIR)/samples/artprov
	ln -sf $(SAMPDIR)/artprov/*.xpm $(DISTDIR)/samples/artprov

	mkdir $(DISTDIR)/samples/calendar
	ln -sf $(SAMPDIR)/calendar/Makefile.in $(DISTDIR)/samples/calendar
	ln -sf $(SAMPDIR)/calendar/makefile.unx $(DISTDIR)/samples/calendar
	ln -sf $(SAMPDIR)/calendar/*.cpp $(DISTDIR)/samples/calendar

	mkdir $(DISTDIR)/samples/caret
	ln -sf $(SAMPDIR)/caret/Makefile.in $(DISTDIR)/samples/caret
	ln -sf $(SAMPDIR)/caret/makefile.unx $(DISTDIR)/samples/caret
	ln -sf $(SAMPDIR)/caret/*.cpp $(DISTDIR)/samples/caret
	ln -sf $(SAMPDIR)/caret/*.xpm $(DISTDIR)/samples/caret

	mkdir $(DISTDIR)/samples/checklst
	ln -sf $(SAMPDIR)/checklst/Makefile.in $(DISTDIR)/samples/checklst
	ln -sf $(SAMPDIR)/checklst/makefile.unx $(DISTDIR)/samples/checklst
	ln -sf $(SAMPDIR)/checklst/*.cpp $(DISTDIR)/samples/checklst
	ln -sf $(SAMPDIR)/checklst/*.xpm $(DISTDIR)/samples/checklst
	ln -sf $(SAMPDIR)/checklst/*.mms $(DISTDIR)/samples/checklst

	mkdir $(DISTDIR)/samples/config
	ln -sf $(SAMPDIR)/config/Makefile.in $(DISTDIR)/samples/config
	ln -sf $(SAMPDIR)/config/makefile.unx $(DISTDIR)/samples/config
	ln -sf $(SAMPDIR)/config/*.cpp $(DISTDIR)/samples/config

	mkdir $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/*.cpp $(DISTDIR)/samples/console
	ln -sf $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

	mkdir $(DISTDIR)/samples/controls
	mkdir $(DISTDIR)/samples/controls/icons
	ln -sf $(SAMPDIR)/controls/Makefile.in $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/makefile.unx $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.cpp $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.xpm $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.bmp $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.ico $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.rc $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/*.mms $(DISTDIR)/samples/controls
	ln -sf $(SAMPDIR)/controls/icons/*.??? $(DISTDIR)/samples/controls/icons

	mkdir $(DISTDIR)/samples/db
	ln -sf $(SAMPDIR)/db/Makefile.in $(DISTDIR)/samples/db
	ln -sf $(SAMPDIR)/db/makefile.unx $(DISTDIR)/samples/db
	ln -sf $(SAMPDIR)/db/*.cpp $(DISTDIR)/samples/db
	ln -sf $(SAMPDIR)/db/*.h $(DISTDIR)/samples/db
	ln -sf $(SAMPDIR)/db/*.xpm $(DISTDIR)/samples/db

	mkdir $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/Makefile.in $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/makefile.unx $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/*.cpp $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/*.h $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/*.txt $(DISTDIR)/samples/dialogs
	ln -sf $(SAMPDIR)/dialogs/*.mms $(DISTDIR)/samples/dialogs

	mkdir $(DISTDIR)/samples/dialup
	ln -sf $(SAMPDIR)/dialup/Makefile.in $(DISTDIR)/samples/dialup
	ln -sf $(SAMPDIR)/dialup/makefile.unx $(DISTDIR)/samples/dialup
	ln -sf $(SAMPDIR)/dialup/*.cpp $(DISTDIR)/samples/dialup

	mkdir $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/Makefile.in $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/makefile.unx $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/*.cpp $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/*.xpm $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/*.txt $(DISTDIR)/samples/dnd
	ln -sf $(SAMPDIR)/dnd/*.png $(DISTDIR)/samples/dnd

	mkdir $(DISTDIR)/samples/docview
	ln -sf $(SAMPDIR)/docview/Makefile.in $(DISTDIR)/samples/docview
	ln -sf $(SAMPDIR)/docview/makefile.unx $(DISTDIR)/samples/docview
	ln -sf $(SAMPDIR)/docview/*.cpp $(DISTDIR)/samples/docview
	ln -sf $(SAMPDIR)/docview/*.h $(DISTDIR)/samples/docview
	ln -sf $(SAMPDIR)/docview/*.xpm $(DISTDIR)/samples/docview

	mkdir $(DISTDIR)/samples/docvwmdi
	ln -sf $(SAMPDIR)/docvwmdi/Makefile.in $(DISTDIR)/samples/docvwmdi
	ln -sf $(SAMPDIR)/docvwmdi/makefile.unx $(DISTDIR)/samples/docvwmdi
	ln -sf $(SAMPDIR)/docvwmdi/*.cpp $(DISTDIR)/samples/docvwmdi
	ln -sf $(SAMPDIR)/docvwmdi/*.h $(DISTDIR)/samples/docvwmdi

	mkdir $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/Makefile.in $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/makefile.unx $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/*.cpp $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/*.h $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/*.xpm $(DISTDIR)/samples/dragimag
	ln -sf $(SAMPDIR)/dragimag/*.png $(DISTDIR)/samples/dragimag

	mkdir $(DISTDIR)/samples/drawing
	ln -sf $(SAMPDIR)/drawing/Makefile.in $(DISTDIR)/samples/drawing
	ln -sf $(SAMPDIR)/drawing/makefile.unx $(DISTDIR)/samples/drawing
	ln -sf $(SAMPDIR)/drawing/*.cpp $(DISTDIR)/samples/drawing
	ln -sf $(SAMPDIR)/drawing/*.xpm $(DISTDIR)/samples/drawing
	ln -sf $(SAMPDIR)/drawing/*.bmp $(DISTDIR)/samples/drawing

	mkdir $(DISTDIR)/samples/dynamic
	ln -sf $(SAMPDIR)/dynamic/Makefile.in $(DISTDIR)/samples/dynamic
	ln -sf $(SAMPDIR)/dynamic/makefile.unx $(DISTDIR)/samples/dynamic
	ln -sf $(SAMPDIR)/dynamic/*.cpp $(DISTDIR)/samples/dynamic
	ln -sf $(SAMPDIR)/dynamic/*.xpm $(DISTDIR)/samples/dynamic

	mkdir $(DISTDIR)/samples/erase
	ln -sf $(SAMPDIR)/erase/Makefile.in $(DISTDIR)/samples/erase
	ln -sf $(SAMPDIR)/erase/makefile.unx $(DISTDIR)/samples/erase
	ln -sf $(SAMPDIR)/erase/*.cpp $(DISTDIR)/samples/erase
	ln -sf $(SAMPDIR)/erase/*.xml $(DISTDIR)/samples/erase

	mkdir $(DISTDIR)/samples/event
	ln -sf $(SAMPDIR)/event/Makefile.in $(DISTDIR)/samples/event
	ln -sf $(SAMPDIR)/event/makefile.unx $(DISTDIR)/samples/event
	ln -sf $(SAMPDIR)/event/*.cpp $(DISTDIR)/samples/event

	mkdir $(DISTDIR)/samples/except
	ln -sf $(SAMPDIR)/except/Makefile.in $(DISTDIR)/samples/except
	ln -sf $(SAMPDIR)/except/*.cpp $(DISTDIR)/samples/except

	mkdir $(DISTDIR)/samples/exec
	ln -sf $(SAMPDIR)/exec/Makefile.in $(DISTDIR)/samples/exec
	ln -sf $(SAMPDIR)/exec/makefile.unx $(DISTDIR)/samples/exec
	ln -sf $(SAMPDIR)/exec/*.cpp $(DISTDIR)/samples/exec

	mkdir $(DISTDIR)/samples/font
	ln -sf $(SAMPDIR)/font/Makefile.in $(DISTDIR)/samples/font
	ln -sf $(SAMPDIR)/font/makefile.unx $(DISTDIR)/samples/font
	ln -sf $(SAMPDIR)/font/*.cpp $(DISTDIR)/samples/font

	mkdir $(DISTDIR)/samples/grid
	ln -sf $(SAMPDIR)/grid/Makefile.in $(DISTDIR)/samples/grid
	ln -sf $(SAMPDIR)/grid/makefile.unx $(DISTDIR)/samples/grid
	ln -sf $(SAMPDIR)/grid/*.cpp $(SAMPDIR)/grid/*.h $(DISTDIR)/samples/grid

	mkdir $(DISTDIR)/samples/help
	mkdir $(DISTDIR)/samples/help/doc
	ln -sf $(SAMPDIR)/help/Makefile.in $(DISTDIR)/samples/help
	ln -sf $(SAMPDIR)/help/makefile.unx $(DISTDIR)/samples/help
	ln -sf $(SAMPDIR)/help/*.cpp $(DISTDIR)/samples/help
	ln -sf $(SAMPDIR)/help/*.xpm $(DISTDIR)/samples/help
	ln -sf $(SAMPDIR)/help/doc/*.* $(DISTDIR)/samples/help/doc

	mkdir $(DISTDIR)/samples/htlbox
	ln -sf $(SAMPDIR)/htlbox/Makefile.in $(DISTDIR)/samples/htlbox
	ln -sf $(SAMPDIR)/htlbox/makefile.unx $(DISTDIR)/samples/htlbox
	ln -sf $(SAMPDIR)/htlbox/*.cpp $(DISTDIR)/samples/htlbox
	ln -sf $(SAMPDIR)/htlbox/*.xpm $(DISTDIR)/samples/htlbox

	mkdir $(DISTDIR)/samples/html
	ln -sf $(SAMPDIR)/html/Makefile.in $(DISTDIR)/samples/html
	mkdir $(DISTDIR)/samples/html/about
	ln -sf $(SAMPDIR)/html/about/Makefile.in $(DISTDIR)/samples/html/about
	ln -sf $(SAMPDIR)/html/about/*.cpp $(DISTDIR)/samples/html/about
	mkdir $(DISTDIR)/samples/html/about/data
	ln -sf $(SAMPDIR)/html/about/data/*.htm $(DISTDIR)/samples/html/about/data
	ln -sf $(SAMPDIR)/html/about/data/*.png $(DISTDIR)/samples/html/about/data
	mkdir $(DISTDIR)/samples/html/help
	ln -sf $(SAMPDIR)/html/help/Makefile.in $(DISTDIR)/samples/html/help
	ln -sf $(SAMPDIR)/html/help/*.cpp $(DISTDIR)/samples/html/help
	mkdir $(DISTDIR)/samples/html/help/helpfiles
	ln -sf $(SAMPDIR)/html/help/helpfiles/*.??? $(DISTDIR)/samples/html/help/helpfiles
	mkdir $(DISTDIR)/samples/html/helpview
	ln -sf $(SAMPDIR)/html/helpview/Makefile.in $(DISTDIR)/samples/html/helpview
	ln -sf $(SAMPDIR)/html/helpview/*.cpp $(DISTDIR)/samples/html/helpview
	ln -sf $(SAMPDIR)/html/helpview/*.zip $(DISTDIR)/samples/html/helpview
	mkdir $(DISTDIR)/samples/html/printing
	ln -sf $(SAMPDIR)/html/printing/Makefile.in $(DISTDIR)/samples/html/printing
	ln -sf $(SAMPDIR)/html/printing/*.cpp $(DISTDIR)/samples/html/printing
	ln -sf $(SAMPDIR)/html/printing/*.htm $(DISTDIR)/samples/html/printing
	mkdir $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/Makefile.in $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.cpp $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.bmp $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.png $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.gif $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.htm $(DISTDIR)/samples/html/test
	ln -sf $(SAMPDIR)/html/test/*.html $(DISTDIR)/samples/html/test
	mkdir $(DISTDIR)/samples/html/virtual
	ln -sf $(SAMPDIR)/html/virtual/Makefile.in $(DISTDIR)/samples/html/virtual
	ln -sf $(SAMPDIR)/html/virtual/*.cpp $(DISTDIR)/samples/html/virtual
	ln -sf $(SAMPDIR)/html/virtual/*.htm $(DISTDIR)/samples/html/virtual
	mkdir $(DISTDIR)/samples/html/widget
	ln -sf $(SAMPDIR)/html/widget/Makefile.in $(DISTDIR)/samples/html/widget
	ln -sf $(SAMPDIR)/html/widget/*.cpp $(DISTDIR)/samples/html/widget
	ln -sf $(SAMPDIR)/html/widget/*.htm $(DISTDIR)/samples/html/widget
	mkdir $(DISTDIR)/samples/html/zip
	ln -sf $(SAMPDIR)/html/zip/Makefile.in $(DISTDIR)/samples/html/zip
	ln -sf $(SAMPDIR)/html/zip/*.cpp $(DISTDIR)/samples/html/zip
	ln -sf $(SAMPDIR)/html/zip/*.htm $(DISTDIR)/samples/html/zip
	ln -sf $(SAMPDIR)/html/zip/*.zip $(DISTDIR)/samples/html/zip

	mkdir $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/Makefile.in $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/makefile.unx $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/*.cpp $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/*.mms $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/horse*.* $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/smile.xbm $(DISTDIR)/samples/image
	ln -sf $(SAMPDIR)/image/smile.xpm $(DISTDIR)/samples/image

	mkdir $(DISTDIR)/samples/internat
	mkdir $(DISTDIR)/samples/internat/bg
	mkdir $(DISTDIR)/samples/internat/cs
	mkdir $(DISTDIR)/samples/internat/de
	mkdir $(DISTDIR)/samples/internat/fr
	mkdir $(DISTDIR)/samples/internat/ja
	mkdir $(DISTDIR)/samples/internat/ka
	mkdir $(DISTDIR)/samples/internat/pl
	mkdir $(DISTDIR)/samples/internat/ru
	mkdir $(DISTDIR)/samples/internat/sv
	ln -sf $(SAMPDIR)/internat/Makefile.in $(DISTDIR)/samples/internat
	ln -sf $(SAMPDIR)/internat/makefile.unx $(DISTDIR)/samples/internat
	ln -sf $(SAMPDIR)/internat/*.cpp $(DISTDIR)/samples/internat
	ln -sf $(SAMPDIR)/internat/*.xpm $(DISTDIR)/samples/internat
	ln -sf $(SAMPDIR)/internat/*.txt $(DISTDIR)/samples/internat
	ln -sf $(SAMPDIR)/internat/bg/*.mo $(DISTDIR)/samples/internat/bg
	ln -sf $(SAMPDIR)/internat/cs/*.mo $(DISTDIR)/samples/internat/cs
	ln -sf $(SAMPDIR)/internat/de/*.mo $(DISTDIR)/samples/internat/de
	ln -sf $(SAMPDIR)/internat/fr/*.mo $(DISTDIR)/samples/internat/fr
	ln -sf $(SAMPDIR)/internat/ja/*.mo $(DISTDIR)/samples/internat/ja
	ln -sf $(SAMPDIR)/internat/ka/*.mo $(DISTDIR)/samples/internat/ka
	ln -sf $(SAMPDIR)/internat/pl/*.mo $(DISTDIR)/samples/internat/pl
	ln -sf $(SAMPDIR)/internat/ru/*.mo $(DISTDIR)/samples/internat/ru
	ln -sf $(SAMPDIR)/internat/sv/*.mo $(DISTDIR)/samples/internat/sv
	ln -sf $(SAMPDIR)/internat/bg/*.po $(DISTDIR)/samples/internat/bg
	ln -sf $(SAMPDIR)/internat/cs/*.po $(DISTDIR)/samples/internat/cs
	ln -sf $(SAMPDIR)/internat/de/*.po $(DISTDIR)/samples/internat/de
	ln -sf $(SAMPDIR)/internat/fr/*.po $(DISTDIR)/samples/internat/fr
	ln -sf $(SAMPDIR)/internat/ja/*.po $(DISTDIR)/samples/internat/ja
	ln -sf $(SAMPDIR)/internat/ka/*.po $(DISTDIR)/samples/internat/ka
	ln -sf $(SAMPDIR)/internat/pl/*.po $(DISTDIR)/samples/internat/pl
	ln -sf $(SAMPDIR)/internat/ru/*.po $(DISTDIR)/samples/internat/ru
	ln -sf $(SAMPDIR)/internat/sv/*.po $(DISTDIR)/samples/internat/sv

	mkdir $(DISTDIR)/samples/ipc
	ln -sf $(SAMPDIR)/ipc/Makefile.in $(DISTDIR)/samples/ipc
	ln -sf $(SAMPDIR)/ipc/makefile.unx $(DISTDIR)/samples/ipc
	ln -sf $(SAMPDIR)/ipc/*.cpp $(DISTDIR)/samples/ipc
	ln -sf $(SAMPDIR)/ipc/*.h $(DISTDIR)/samples/ipc
	ln -sf $(SAMPDIR)/ipc/*.xpm $(DISTDIR)/samples/ipc

	mkdir $(DISTDIR)/samples/joytest
	ln -sf $(SAMPDIR)/joytest/Makefile.in $(DISTDIR)/samples/joytest
	ln -sf $(SAMPDIR)/joytest/makefile.unx $(DISTDIR)/samples/joytest
	ln -sf $(SAMPDIR)/joytest/*.cpp $(DISTDIR)/samples/joytest
	ln -sf $(SAMPDIR)/joytest/*.h $(DISTDIR)/samples/joytest
	ln -sf $(SAMPDIR)/joytest/*.wav $(DISTDIR)/samples/joytest

	mkdir $(DISTDIR)/samples/keyboard
	ln -sf $(SAMPDIR)/keyboard/Makefile.in $(DISTDIR)/samples/keyboard
	ln -sf $(SAMPDIR)/keyboard/*.cpp $(DISTDIR)/samples/keyboard

	mkdir $(DISTDIR)/samples/layout
	ln -sf $(SAMPDIR)/layout/Makefile.in $(DISTDIR)/samples/layout
	ln -sf $(SAMPDIR)/layout/makefile.unx $(DISTDIR)/samples/layout
	ln -sf $(SAMPDIR)/layout/*.cpp $(DISTDIR)/samples/layout
	ln -sf $(SAMPDIR)/layout/*.h $(DISTDIR)/samples/layout

	mkdir $(DISTDIR)/samples/listctrl
	mkdir $(DISTDIR)/samples/listctrl/bitmaps
	ln -sf $(SAMPDIR)/listctrl/Makefile.in $(DISTDIR)/samples/listctrl
	ln -sf $(SAMPDIR)/listctrl/makefile.unx $(DISTDIR)/samples/listctrl
	ln -sf $(SAMPDIR)/listctrl/*.cpp $(DISTDIR)/samples/listctrl
	ln -sf $(SAMPDIR)/listctrl/*.h $(DISTDIR)/samples/listctrl
	ln -sf $(SAMPDIR)/listctrl/*.xpm $(DISTDIR)/samples/listctrl
	ln -sf $(SAMPDIR)/listctrl/bitmaps/*.xpm $(DISTDIR)/samples/listctrl/bitmaps

	mkdir $(DISTDIR)/samples/mediaplayer
	ln -sf $(SAMPDIR)/mediaplayer/Makefile.in $(DISTDIR)/samples/mediaplayer
	ln -sf $(SAMPDIR)/mediaplayer/*.cpp $(DISTDIR)/samples/mediaplayer

	mkdir $(DISTDIR)/samples/mdi
	mkdir $(DISTDIR)/samples/mdi/bitmaps
	ln -sf $(SAMPDIR)/mdi/Makefile.in $(DISTDIR)/samples/mdi
	ln -sf $(SAMPDIR)/mdi/makefile.unx $(DISTDIR)/samples/mdi
	ln -sf $(SAMPDIR)/mdi/*.cpp $(DISTDIR)/samples/mdi
	ln -sf $(SAMPDIR)/mdi/*.h $(DISTDIR)/samples/mdi
	ln -sf $(SAMPDIR)/mdi/*.xpm $(DISTDIR)/samples/mdi
	ln -sf $(SAMPDIR)/mdi/bitmaps/*.xpm $(DISTDIR)/samples/mdi/bitmaps

	mkdir $(DISTDIR)/samples/memcheck
	ln -sf $(SAMPDIR)/memcheck/Makefile.in $(DISTDIR)/samples/memcheck
	ln -sf $(SAMPDIR)/memcheck/makefile.unx $(DISTDIR)/samples/memcheck
	ln -sf $(SAMPDIR)/memcheck/*.cpp $(DISTDIR)/samples/memcheck
	ln -sf $(SAMPDIR)/memcheck/*.xpm $(DISTDIR)/samples/memcheck

	mkdir $(DISTDIR)/samples/menu
	ln -sf $(SAMPDIR)/menu/Makefile.in $(DISTDIR)/samples/menu
	ln -sf $(SAMPDIR)/menu/makefile.unx $(DISTDIR)/samples/menu
	ln -sf $(SAMPDIR)/menu/*.cpp $(DISTDIR)/samples/menu
	ln -sf $(SAMPDIR)/menu/*.xpm $(DISTDIR)/samples/menu

	mkdir $(DISTDIR)/samples/minifram
	mkdir $(DISTDIR)/samples/minifram/bitmaps
	ln -sf $(SAMPDIR)/minifram/Makefile.in $(DISTDIR)/samples/minifram
	ln -sf $(SAMPDIR)/minifram/makefile.unx $(DISTDIR)/samples/minifram
	ln -sf $(SAMPDIR)/minifram/*.cpp $(DISTDIR)/samples/minifram
	ln -sf $(SAMPDIR)/minifram/*.h $(DISTDIR)/samples/minifram
	ln -sf $(SAMPDIR)/minifram/*.xpm $(DISTDIR)/samples/minifram
	ln -sf $(SAMPDIR)/minifram/bitmaps/*.xpm $(DISTDIR)/samples/minifram/bitmaps

	mkdir $(DISTDIR)/samples/minimal
	ln -sf $(SAMPDIR)/minimal/Makefile.in $(DISTDIR)/samples/minimal
	ln -sf $(SAMPDIR)/minimal/makefile.unx $(DISTDIR)/samples/minimal
	ln -sf $(SAMPDIR)/minimal/*.cpp $(DISTDIR)/samples/minimal
	ln -sf $(SAMPDIR)/minimal/*.mms $(DISTDIR)/samples/minimal

	mkdir $(DISTDIR)/samples/mobile
	ln -sf $(SAMPDIR)/mobile/Makefile.in $(DISTDIR)/samples/mobile
	mkdir $(DISTDIR)/samples/mobile/wxedit
	ln -sf $(SAMPDIR)/mobile/wxedit/Makefile.in $(DISTDIR)/samples/mobile/wxedit
	ln -sf $(SAMPDIR)/mobile/wxedit/*.cpp $(DISTDIR)/samples/mobile/wxedit
	ln -sf $(SAMPDIR)/mobile/wxedit/*.h $(DISTDIR)/samples/mobile/wxedit
	mkdir $(DISTDIR)/samples/mobile/styles
	ln -sf $(SAMPDIR)/mobile/styles/Makefile.in $(DISTDIR)/samples/mobile/styles
	ln -sf $(SAMPDIR)/mobile/styles/*.cpp $(DISTDIR)/samples/mobile/styles
	ln -sf $(SAMPDIR)/mobile/styles/*.h $(DISTDIR)/samples/mobile/styles
	ln -sf $(SAMPDIR)/mobile/styles/*.jpg $(DISTDIR)/samples/mobile/styles

	mkdir $(DISTDIR)/samples/multimon
	ln -sf $(SAMPDIR)/multimon/Makefile.in $(DISTDIR)/samples/multimon
	ln -sf $(SAMPDIR)/multimon/makefile.unx $(DISTDIR)/samples/multimon
	ln -sf $(SAMPDIR)/multimon/*.cpp $(DISTDIR)/samples/multimon

	mkdir $(DISTDIR)/samples/notebook
	ln -sf $(SAMPDIR)/notebook/Makefile.in $(DISTDIR)/samples/notebook
	ln -sf $(SAMPDIR)/notebook/makefile.unx $(DISTDIR)/samples/notebook
	ln -sf $(SAMPDIR)/notebook/*.cpp $(DISTDIR)/samples/notebook
	ln -sf $(SAMPDIR)/notebook/*.h $(DISTDIR)/samples/notebook

	mkdir $(DISTDIR)/samples/opengl
	ln -sf $(SAMPDIR)/opengl/Makefile.in $(DISTDIR)/samples/opengl
	mkdir $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/Makefile.in $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/makefile.unx $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/*.cpp $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/*.c $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/*.h $(DISTDIR)/samples/opengl/penguin
	ln -sf $(SAMPDIR)/opengl/penguin/*.lwo $(DISTDIR)/samples/opengl/penguin
	mkdir $(DISTDIR)/samples/opengl/cube
	ln -sf $(SAMPDIR)/opengl/cube/Makefile.in $(DISTDIR)/samples/opengl/cube
	ln -sf $(SAMPDIR)/opengl/cube/makefile.unx $(DISTDIR)/samples/opengl/cube
	ln -sf $(SAMPDIR)/opengl/cube/*.cpp $(DISTDIR)/samples/opengl/cube
	ln -sf $(SAMPDIR)/opengl/cube/*.h $(DISTDIR)/samples/opengl/cube
	mkdir $(DISTDIR)/samples/opengl/isosurf
	ln -sf $(SAMPDIR)/opengl/isosurf/Makefile.in $(DISTDIR)/samples/opengl/isosurf
	ln -sf $(SAMPDIR)/opengl/isosurf/makefile.unx $(DISTDIR)/samples/opengl/isosurf
	ln -sf $(SAMPDIR)/opengl/isosurf/*.cpp $(DISTDIR)/samples/opengl/isosurf
	ln -sf $(SAMPDIR)/opengl/isosurf/*.h $(DISTDIR)/samples/opengl/isosurf
	ln -sf $(SAMPDIR)/opengl/isosurf/*.gz $(DISTDIR)/samples/opengl/isosurf

	mkdir $(DISTDIR)/samples/png
	ln -sf $(SAMPDIR)/png/Makefile.in $(DISTDIR)/samples/png
	ln -sf $(SAMPDIR)/png/makefile.unx $(DISTDIR)/samples/png
	ln -sf $(SAMPDIR)/png/*.cpp $(DISTDIR)/samples/png
	ln -sf $(SAMPDIR)/png/*.h $(DISTDIR)/samples/png
	ln -sf $(SAMPDIR)/png/*.png $(DISTDIR)/samples/png

	mkdir $(DISTDIR)/samples/popup
	ln -sf $(SAMPDIR)/popup/Makefile.in $(DISTDIR)/samples/popup
	ln -sf $(SAMPDIR)/popup/makefile.unx $(DISTDIR)/samples/popup
	ln -sf $(SAMPDIR)/popup/*.cpp $(DISTDIR)/samples/popup

	mkdir $(DISTDIR)/samples/printing
	ln -sf $(SAMPDIR)/printing/Makefile.in $(DISTDIR)/samples/printing
	ln -sf $(SAMPDIR)/printing/makefile.unx $(DISTDIR)/samples/printing
	ln -sf $(SAMPDIR)/printing/*.cpp $(DISTDIR)/samples/printing
	ln -sf $(SAMPDIR)/printing/*.h $(DISTDIR)/samples/printing
	ln -sf $(SAMPDIR)/printing/*.xpm $(DISTDIR)/samples/printing

	mkdir $(DISTDIR)/samples/render
	ln -sf $(SAMPDIR)/render/Makefile.in $(DISTDIR)/samples/render
	ln -sf $(SAMPDIR)/render/makefile.unx $(DISTDIR)/samples/render
	ln -sf $(SAMPDIR)/render/*.cpp $(DISTDIR)/samples/render

	mkdir $(DISTDIR)/samples/rotate
	ln -sf $(SAMPDIR)/rotate/Makefile.in $(DISTDIR)/samples/rotate
	ln -sf $(SAMPDIR)/rotate/makefile.unx $(DISTDIR)/samples/rotate
	ln -sf $(SAMPDIR)/rotate/*.cpp $(DISTDIR)/samples/rotate
	ln -sf $(SAMPDIR)/rotate/*.png $(DISTDIR)/samples/rotate

	mkdir $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/Makefile.in $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/makefile.unx $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/*.cpp $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/*.h $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/*.xpm $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/README $(DISTDIR)/samples/richedit
	ln -sf $(SAMPDIR)/richedit/TODO $(DISTDIR)/samples/richedit

	mkdir $(DISTDIR)/samples/propsize
	ln -sf $(SAMPDIR)/propsize/Makefile.in $(DISTDIR)/samples/propsize
	ln -sf $(SAMPDIR)/propsize/makefile.unx $(DISTDIR)/samples/propsize
	ln -sf $(SAMPDIR)/propsize/*.cpp $(DISTDIR)/samples/propsize
	ln -sf $(SAMPDIR)/propsize/*.xpm $(DISTDIR)/samples/propsize

	mkdir $(DISTDIR)/samples/sashtest
	ln -sf $(SAMPDIR)/sashtest/Makefile.in $(DISTDIR)/samples/sashtest
	ln -sf $(SAMPDIR)/sashtest/makefile.unx $(DISTDIR)/samples/sashtest
	ln -sf $(SAMPDIR)/sashtest/*.cpp $(DISTDIR)/samples/sashtest
	ln -sf $(SAMPDIR)/sashtest/*.h $(DISTDIR)/samples/sashtest

	mkdir $(DISTDIR)/samples/scroll
	ln -sf $(SAMPDIR)/scroll/Makefile.in $(DISTDIR)/samples/scroll
	ln -sf $(SAMPDIR)/scroll/makefile.unx $(DISTDIR)/samples/scroll
	ln -sf $(SAMPDIR)/scroll/*.cpp $(DISTDIR)/samples/scroll

	mkdir $(DISTDIR)/samples/scrollsub
	ln -sf $(SAMPDIR)/scrollsub/Makefile.in $(DISTDIR)/samples/scrollsub
	ln -sf $(SAMPDIR)/scrollsub/makefile.unx $(DISTDIR)/samples/scrollsub
	ln -sf $(SAMPDIR)/scrollsub/*.cpp $(DISTDIR)/samples/scrollsub

	mkdir $(DISTDIR)/samples/shaped
	ln -sf $(SAMPDIR)/shaped/Makefile.in $(DISTDIR)/samples/shaped
	ln -sf $(SAMPDIR)/shaped/makefile.unx $(DISTDIR)/samples/shaped
	ln -sf $(SAMPDIR)/shaped/*.cpp $(DISTDIR)/samples/shaped
	ln -sf $(SAMPDIR)/shaped/*.png $(DISTDIR)/samples/shaped

	mkdir $(DISTDIR)/samples/sockets
	ln -sf $(SAMPDIR)/sockets/Makefile.in $(DISTDIR)/samples/sockets
	ln -sf $(SAMPDIR)/sockets/makefile.unx $(DISTDIR)/samples/sockets
	ln -sf $(SAMPDIR)/sockets/*.cpp $(DISTDIR)/samples/sockets
	ln -sf $(SAMPDIR)/sockets/*.xpm $(DISTDIR)/samples/sockets

	mkdir $(DISTDIR)/samples/sound
	ln -sf $(SAMPDIR)/sound/Makefile.in $(DISTDIR)/samples/sound
	ln -sf $(SAMPDIR)/sound/*.cpp $(DISTDIR)/samples/sound
	ln -sf $(SAMPDIR)/sound/*.wav $(DISTDIR)/samples/sound

	mkdir $(DISTDIR)/samples/splash
	ln -sf $(SAMPDIR)/splash/Makefile.in $(DISTDIR)/samples/splash
	ln -sf $(SAMPDIR)/splash/*.cpp $(DISTDIR)/samples/splash
	ln -sf $(SAMPDIR)/splash/*.png $(DISTDIR)/samples/splash
	ln -sf $(SAMPDIR)/splash/*.mpg $(DISTDIR)/samples/splash

	mkdir $(DISTDIR)/samples/splitter
	ln -sf $(SAMPDIR)/splitter/Makefile.in $(DISTDIR)/samples/splitter
	ln -sf $(SAMPDIR)/splitter/makefile.unx $(DISTDIR)/samples/splitter
	ln -sf $(SAMPDIR)/splitter/*.cpp $(DISTDIR)/samples/splitter

	mkdir $(DISTDIR)/samples/statbar
	ln -sf $(SAMPDIR)/statbar/Makefile.in $(DISTDIR)/samples/statbar
	ln -sf $(SAMPDIR)/statbar/makefile.unx $(DISTDIR)/samples/statbar
	ln -sf $(SAMPDIR)/statbar/*.cpp $(DISTDIR)/samples/statbar
	ln -sf $(SAMPDIR)/statbar/*.xpm $(DISTDIR)/samples/statbar

	mkdir $(DISTDIR)/samples/text
	ln -sf $(SAMPDIR)/text/Makefile.in $(DISTDIR)/samples/text
	ln -sf $(SAMPDIR)/text/makefile.unx $(DISTDIR)/samples/text
	ln -sf $(SAMPDIR)/text/*.cpp $(DISTDIR)/samples/text
	ln -sf $(SAMPDIR)/text/*.xpm $(DISTDIR)/samples/text

	mkdir $(DISTDIR)/samples/thread
	ln -sf $(SAMPDIR)/thread/Makefile.in $(DISTDIR)/samples/thread
	ln -sf $(SAMPDIR)/thread/makefile.unx $(DISTDIR)/samples/thread
	ln -sf $(SAMPDIR)/thread/*.cpp $(DISTDIR)/samples/thread

	mkdir $(DISTDIR)/samples/toolbar
	ln -sf $(SAMPDIR)/toolbar/Makefile.in $(DISTDIR)/samples/toolbar
	ln -sf $(SAMPDIR)/toolbar/makefile.unx $(DISTDIR)/samples/toolbar
	ln -sf $(SAMPDIR)/toolbar/*.cpp $(DISTDIR)/samples/toolbar
	ln -sf $(SAMPDIR)/toolbar/*.xpm $(DISTDIR)/samples/toolbar
	mkdir $(DISTDIR)/samples/toolbar/bitmaps
	ln -sf $(SAMPDIR)/toolbar/bitmaps/*.xpm $(DISTDIR)/samples/toolbar/bitmaps

	mkdir $(DISTDIR)/samples/treectrl
	ln -sf $(SAMPDIR)/treectrl/Makefile.in $(DISTDIR)/samples/treectrl
	ln -sf $(SAMPDIR)/treectrl/makefile.unx $(DISTDIR)/samples/treectrl
	ln -sf $(SAMPDIR)/treectrl/*.cpp $(DISTDIR)/samples/treectrl
	ln -sf $(SAMPDIR)/treectrl/*.h $(DISTDIR)/samples/treectrl
	ln -sf $(SAMPDIR)/treectrl/*.xpm $(DISTDIR)/samples/treectrl

	mkdir $(DISTDIR)/samples/typetest
	ln -sf $(SAMPDIR)/typetest/Makefile.in $(DISTDIR)/samples/typetest
	ln -sf $(SAMPDIR)/typetest/makefile.unx $(DISTDIR)/samples/typetest
	ln -sf $(SAMPDIR)/typetest/*.cpp $(DISTDIR)/samples/typetest
	ln -sf $(SAMPDIR)/typetest/*.h $(DISTDIR)/samples/typetest
	ln -sf $(SAMPDIR)/typetest/*.xpm $(DISTDIR)/samples/typetest

	mkdir $(DISTDIR)/samples/validate
	ln -sf $(SAMPDIR)/validate/Makefile.in $(DISTDIR)/samples/validate
	ln -sf $(SAMPDIR)/validate/makefile.unx $(DISTDIR)/samples/validate
	ln -sf $(SAMPDIR)/validate/*.cpp $(DISTDIR)/samples/validate
	ln -sf $(SAMPDIR)/validate/*.h $(DISTDIR)/samples/validate
	ln -sf $(SAMPDIR)/validate/*.xpm $(DISTDIR)/samples/validate

	mkdir $(DISTDIR)/samples/vscroll
	ln -sf $(SAMPDIR)/vscroll/Makefile.in $(DISTDIR)/samples/vscroll
	ln -sf $(SAMPDIR)/vscroll/*.cpp $(DISTDIR)/samples/vscroll

	mkdir $(DISTDIR)/samples/wizard
	ln -sf $(SAMPDIR)/wizard/Makefile.in $(DISTDIR)/samples/wizard
	ln -sf $(SAMPDIR)/wizard/makefile.unx $(DISTDIR)/samples/wizard
	ln -sf $(SAMPDIR)/wizard/*.cpp $(DISTDIR)/samples/wizard
	ln -sf $(SAMPDIR)/wizard/*.xpm $(DISTDIR)/samples/wizard

	mkdir $(DISTDIR)/samples/widgets
	mkdir $(DISTDIR)/samples/widgets/icons
	ln -sf $(SAMPDIR)/widgets/Makefile.in $(DISTDIR)/samples/widgets
	ln -sf $(SAMPDIR)/widgets/*.cpp $(DISTDIR)/samples/widgets
	ln -sf $(SAMPDIR)/widgets/*.h $(DISTDIR)/samples/widgets
	ln -sf $(SAMPDIR)/widgets/*.rc $(DISTDIR)/samples/widgets
	ln -sf $(SAMPDIR)/widgets/icons/*.xpm $(DISTDIR)/samples/widgets/icons

	mkdir $(DISTDIR)/samples/xrc
	mkdir $(DISTDIR)/samples/xrc/rc
	ln -sf $(SAMPDIR)/xrc/Makefile.in $(DISTDIR)/samples/xrc
	ln -sf $(SAMPDIR)/xrc/*.cpp $(DISTDIR)/samples/xrc
	ln -sf $(SAMPDIR)/xrc/*.h $(DISTDIR)/samples/xrc
	ln -sf $(SAMPDIR)/xrc/*.rc $(DISTDIR)/samples/xrc
	ln -sf $(SAMPDIR)/xrc/rc/*.xpm $(DISTDIR)/samples/xrc/rc
	ln -sf $(SAMPDIR)/xrc/rc/*.xrc $(DISTDIR)/samples/xrc/rc
	ln -sf $(SAMPDIR)/xrc/rc/*.gif $(DISTDIR)/samples/xrc/rc
	ln -sf $(SAMPDIR)/xrc/rc/*.ico $(DISTDIR)/samples/xrc/rc

UTILS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/utils
	ln -sf $(UTILSDIR)/Makefile.in $(DISTDIR)/utils

	mkdir $(DISTDIR)/utils/tex2rtf
	mkdir $(DISTDIR)/utils/tex2rtf/src
	ln -sf $(UTILSDIR)/tex2rtf/*.in $(DISTDIR)/utils/tex2rtf
	ln -sf $(UTILSDIR)/tex2rtf/src/*.h $(DISTDIR)/utils/tex2rtf/src
	ln -sf $(UTILSDIR)/tex2rtf/src/*.in $(DISTDIR)/utils/tex2rtf/src
	ln -sf $(UTILSDIR)/tex2rtf/src/*.cpp $(DISTDIR)/utils/tex2rtf/src
	ln -sf $(UTILSDIR)/tex2rtf/src/tex2rtf.* $(DISTDIR)/utils/tex2rtf/src

	mkdir $(DISTDIR)/utils/emulator
	mkdir $(DISTDIR)/utils/emulator/src
	mkdir $(DISTDIR)/utils/emulator/docs
	ln -sf $(UTILSDIR)/emulator/*.in $(DISTDIR)/utils/emulator
	ln -sf $(UTILSDIR)/emulator/src/*.h $(DISTDIR)/utils/emulator/src
	ln -sf $(UTILSDIR)/emulator/src/*.in $(DISTDIR)/utils/emulator/src
	ln -sf $(UTILSDIR)/emulator/src/*.cpp $(DISTDIR)/utils/emulator/src
	ln -sf $(UTILSDIR)/emulator/src/*.jpg $(DISTDIR)/utils/emulator/src
	ln -sf $(UTILSDIR)/emulator/src/*.wxe $(DISTDIR)/utils/emulator/src
	ln -sf $(UTILSDIR)/emulator/docs/*.txt $(DISTDIR)/utils/emulator/docs
	ln -sf $(UTILSDIR)/emulator/docs/*.jpg $(DISTDIR)/utils/emulator/docs

	mkdir $(DISTDIR)/utils/hhp2cached
	ln -sf $(UTILSDIR)/hhp2cached/Makefile.in $(DISTDIR)/utils/hhp2cached
	ln -sf $(UTILSDIR)/hhp2cached/*.cpp $(DISTDIR)/utils/hhp2cached
	ln -sf $(UTILSDIR)/hhp2cached/*.rc $(DISTDIR)/utils/hhp2cached

	mkdir $(DISTDIR)/utils/HelpGen
	mkdir $(DISTDIR)/utils/HelpGen/src
	ln -sf $(UTILSDIR)/HelpGen/Makefile.in $(DISTDIR)/utils/HelpGen
	ln -sf $(UTILSDIR)/HelpGen/src/Makefile.in $(DISTDIR)/utils/HelpGen/src
	ln -sf $(UTILSDIR)/HelpGen/src/*.h $(DISTDIR)/utils/HelpGen/src
	ln -sf $(UTILSDIR)/HelpGen/src/*.cpp $(DISTDIR)/utils/HelpGen/src

	mkdir $(DISTDIR)/utils/helpview
	mkdir $(DISTDIR)/utils/helpview/src
	mkdir $(DISTDIR)/utils/helpview/src/bitmaps
	ln -sf $(UTILSDIR)/helpview/Makefile.in $(DISTDIR)/utils/helpview
	ln -sf $(UTILSDIR)/helpview/src/*.h $(DISTDIR)/utils/helpview/src
	ln -sf $(UTILSDIR)/helpview/src/*.cpp $(DISTDIR)/utils/helpview/src
	ln -sf $(UTILSDIR)/helpview/src/Makefile.in $(DISTDIR)/utils/helpview/src
	ln -sf $(UTILSDIR)/helpview/src/test.zip $(DISTDIR)/utils/helpview/src
	ln -sf $(UTILSDIR)/helpview/src/bitmaps/*.xpm $(DISTDIR)/utils/helpview/src/bitmaps

	mkdir $(DISTDIR)/utils/wxrc
	ln -sf $(UTILSDIR)/wxrc/Makefile.in $(DISTDIR)/utils/wxrc
	ln -sf $(UTILSDIR)/wxrc/*.cpp $(DISTDIR)/utils/wxrc
	ln -sf $(UTILSDIR)/wxrc/*.rc $(DISTDIR)/utils/wxrc

MISC_DIST: ALL_GUI_DIST

INTL_DIST:
	mkdir $(DISTDIR)/locale
	ln -sf $(INTLDIR)/Makefile $(DISTDIR)/locale
	ln -sf $(INTLDIR)/*.po $(DISTDIR)/locale
	-ln -sf $(INTLDIR)/*.mo $(DISTDIR)/locale
	subdirs=`cd $(INTLDIR) && ls */*.po | sed 's|/.*||' | uniq`; \
	for dir in "$$subdirs"; do                                   \
	    mkdir $(DISTDIR)/locale/$$dir;                           \
	    ln -sf $(INTLDIR)/$$dir/*.[pm]o $(DISTDIR)/locale/$$dir;     \
	done

MANUAL_DIST:
	mkdir $(DISTDIR)/docs
	mkdir $(DISTDIR)/docs/latex
	mkdir $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.tex $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.inc $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.gif $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.ini $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.bib $(DISTDIR)/docs/latex/wx
	ln -sf $(DOCDIR)/latex/wx/*.sty $(DISTDIR)/docs/latex/wx


# Copy all the files from wxPython needed for the Debian source package,
# and then remove some that are not needed.
PYTHON_DIST:
	for dir in `grep -v '#' $(WXDIR)/wxPython/distrib/DIRLIST`; do \
		echo "Copying dir: $$dir..."; \
		mkdir $(DISTDIR)/$$dir; \
		ln -sf $(WXDIR)/$$dir/* $(DISTDIR)/$$dir > /dev/null 2>&1; \
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

distdir: @GUIDIST@
	@echo "*** Creating wxWidgets distribution in $(DISTDIR)..."
	@# now prune away a lot of the crap included by using cp -R
	@# in other dist targets.
	find $(DISTDIR) \( -name "CVS" -o -name ".cvsignore" -o -name "*.dsp" -o -name "*.dsw" -o -name "*.hh*" -o \
			\( -name "makefile.*" -a ! -name "makefile.unx" \) \) \
			-print0 | xargs -0 rm -rf

dist: distdir
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
	ln -sf $(INCDIR)/wx/msw/*.cur _dist_dir/wxMSW/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.ico _dist_dir/wxMSW/include/wx/msw
	ln -sf $(INCDIR)/wx/msw/*.bmp _dist_dir/wxMSW/include/wx/msw
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.cur
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.ico
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.bmp

debian-dist: DEBIAN_SOURCE_DIR = $(WXDIR)/../wxwidgets@WX_RELEASE@@WX_FLAVOUR@-@WX_SUBVERSION@
debian-dist: debian-native-dist debian-msw-dirs MSW_DIST
	mkdir $(DISTDIR)/debian
	-ln -sf $(WXDIR)/debian/* $(DISTDIR)/debian
	ln -sf $(DOCDIR)/licence.txt $(DISTDIR)/docs
	ln -sf $(DOCDIR)/licendoc.txt $(DISTDIR)/docs
	ln -sf $(DOCDIR)/preamble.txt $(DISTDIR)/docs
	rm -f $(DISTDIR)/*.spec

	@# now prune away a lot of the crap included by using cp -R
	@# in other dist targets.  Ugly and hardly portable but it
	@# will run on any Debian box and that's enough for now.

	rm -rf $(DISTDIR)/contrib/build
	find $(DISTDIR) \( -name "CVS" -o -name ".cvsignore" -o -name "*.dsp"    \
			   -o -name "*.dsw" -o -name "*.hh*" -o -name "*.mms"    \
			   -o -name "*.mcp" -o -name "*M*.xml" -o -name "*.r"    \
			   -o -name "*.bkl" -o -name "*.pro"  \
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
