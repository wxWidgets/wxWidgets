#
# "make dist" target implementation:
#


############################# Dirs #################################

WXDIR = @abs_top_srcdir@

# Subordinate library possibilities

SRCDIR   = $(WXDIR)/src
GENDIR   = $(WXDIR)/src/generic
COMMDIR  = $(WXDIR)/src/common
HTMLDIR  = $(WXDIR)/src/html
RICHTEXTDIR = $(WXDIR)/src/richtext
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

########################## Tools ###############################

# Links can be used instead of copying to make it a little quicker and use
# a little less disk space. Set LNDIR='cp -R' and LN_SF=cp to revert
# to the old behaviour of copying.
LNDIR = @LNDIR@
LN_SF = $(LN_S) -f

########################## make dist rules ###############################

# this target should copy only the files common to really all ports (including
# wxBase) to the dist dir
ALL_DIST: distrib_clean
	mkdir _dist_dir
	mkdir $(DISTDIR)
	$(LN_SF) $(WXDIR)/configure.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/configure $(DISTDIR)
	$(LN_SF) $(WXDIR)/autoconf_inc.m4 $(DISTDIR)
	$(LN_SF) $(WXDIR)/wxwin.m4 $(DISTDIR)
	$(LN_SF) $(WXDIR)/aclocal.m4 $(DISTDIR)
	$(LN_SF) $(WXDIR)/config.sub $(DISTDIR)
	$(LN_SF) $(WXDIR)/config.guess $(DISTDIR)
	$(LN_SF) $(WXDIR)/install-sh $(DISTDIR)
	$(LN_SF) $(WXDIR)/mkinstalldirs $(DISTDIR)
	$(LN_SF) $(WXDIR)/wx-config.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/wx-config-inplace.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/version-script.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/setup.h.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/setup.h_vms $(DISTDIR)
	$(LN_SF) $(WXDIR)/descrip.mms $(DISTDIR)
	$(LN_SF) $(WXDIR)/Makefile.in $(DISTDIR)
	$(LN_SF) $(WXDIR)/wxBase.spec $(DISTDIR)
	$(LN_SF) $(DOCDIR)/lgpl.txt $(DISTDIR)/COPYING.LIB
	$(LN_SF) $(DOCDIR)/licence.txt $(DISTDIR)/LICENCE.txt
	$(LN_SF) $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	mkdir $(DISTDIR)/lib
	$(LN_SF) $(WXDIR)/lib/vms.opt $(DISTDIR)/lib
	$(LN_SF) $(WXDIR)/lib/vms_gtk.opt $(DISTDIR)/lib
	mkdir $(DISTDIR)/src
	# temp hack for common/execcmn.cpp: it's not supported by tmake
	# yet (it's a header-like file but in src/common directory and it
	# shouldn't be distributed...)
	mkdir $(DISTDIR)/src/common
	$(LN_SF) $(SRCDIR)/common/execcmn.cpp $(DISTDIR)/src/common
	mkdir $(DISTDIR)/src/xml
	$(LN_SF) $(SRCDIR)/xml/*.cpp $(DISTDIR)/src/xml
	mkdir $(DISTDIR)/src/zlib
	$(LN_SF) $(ZLIBDIR)/*.h $(DISTDIR)/src/zlib
	$(LN_SF) $(ZLIBDIR)/*.c $(DISTDIR)/src/zlib
	$(LN_SF) $(ZLIBDIR)/README $(DISTDIR)/src/zlib
	#$(LN_SF) $(ZLIBDIR)/*.mms $(DISTDIR)/src/zlib
	mkdir $(DISTDIR)/src/regex
	$(LN_SF) $(REGEXDIR)/*.h $(DISTDIR)/src/regex
	$(LN_SF) $(REGEXDIR)/*.c $(DISTDIR)/src/regex
	$(LN_SF) $(REGEXDIR)/COPYRIGHT $(DISTDIR)/src/regex
	$(LN_SF) $(REGEXDIR)/README $(DISTDIR)/src/regex
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/src/expat
	$(LNDIR) $(EXPATDIR) $(DISTDIR)/src/expat
	#(cd $(DISTDIR)/src/expat ; rm -rf `find -name CVS`)
	mkdir $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/*.h $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/*.c $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/*.ci $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/*.exp $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/README $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/NEWS $(DISTDIR)/src/iodbc
	$(LN_SF) $(ODBCDIR)/Changes.log $(DISTDIR)/src/iodbc
	# copy some files from include/ that are not installed:
	mkdir $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx
	$(LN_SF) $(INCDIR)/wx/fmappriv.h $(DISTDIR)/include/wx
	# copy wxpresets
	mkdir $(DISTDIR)/build
	mkdir $(DISTDIR)/build/bakefiles
	mkdir $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/presets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(LN_SF) $(WXDIR)/build/bakefiles/wxpresets/presets/*.bkl $(DISTDIR)/build/bakefiles/wxpresets/presets
	$(LN_SF) $(WXDIR)/build/bakefiles/wxpresets/sample/minimal* $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(LN_SF) $(WXDIR)/build/bakefiles/wxpresets/sample/config* $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(LN_SF) $(WXDIR)/build/bakefiles/wxpresets/*.txt $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/aclocal
	$(LN_SF) $(WXDIR)/build/aclocal/*.m4 $(DISTDIR)/build/aclocal

# this target is the common part of distribution script for all GUI toolkits,
# but is not used when building wxBase distribution
ALL_GUI_DIST: ALL_DIST
	$(LN_SF) $(DOCDIR)/readme.txt $(DISTDIR)/README.txt
	$(LN_SF) $(DOCDIR)/$(TOOLKITDIR)/install.txt $(DISTDIR)/INSTALL.txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/changes.txt ; then \
	  $(LN_SF) $(DOCDIR)/$(TOOLKITDIR)/changes.txt $(DISTDIR)/CHANGES-$(TOOLKIT).txt ; fi
	$(LN_SF) $(DOCDIR)/$(TOOLKITDIR)/readme.txt $(DISTDIR)/README-$(TOOLKIT).txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/todo.txt ; then \
	  $(LN_SF) $(DOCDIR)/$(TOOLKITDIR)/todo.txt $(DISTDIR)/TODO.txt ; fi
	mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)
	mkdir $(DISTDIR)/include/wx/generic
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/richtext
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/xrc
	-$(LN_SF) $(INCDIR)/wx/*.h $(DISTDIR)/include/wx
	$(LN_SF) $(INCDIR)/wx/*.cpp $(DISTDIR)/include/wx
	$(LN_SF) $(INCDIR)/wx/generic/*.h $(DISTDIR)/include/wx/generic
	$(LN_SF) $(INCDIR)/wx/html/*.h $(DISTDIR)/include/wx/html
	$(LN_SF) $(INCDIR)/wx/richtext/*.h $(DISTDIR)/include/wx/richtext
	$(LN_SF) $(INCDIR)/wx/unix/*.h $(DISTDIR)/include/wx/unix
	$(LN_SF) $(INCDIR)/wx/xml/*.h $(DISTDIR)/include/wx/xml
	$(LN_SF) $(INCDIR)/wx/xrc/*.h $(DISTDIR)/include/wx/xrc
	$(LN_SF) $(INCDIR)/wx/protocol/*.h $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/art
	mkdir $(DISTDIR)/art/gtk
	mkdir $(DISTDIR)/art/motif
	$(LN_SF) $(WXDIR)/art/*.xpm $(DISTDIR)/art
	$(LN_SF) $(WXDIR)/art/gtk/*.xpm $(DISTDIR)/art/gtk
	$(LN_SF) $(WXDIR)/art/motif/*.xpm $(DISTDIR)/art/motif
	mkdir $(DISTDIR)/src/generic
	mkdir $(DISTDIR)/src/html
	mkdir $(DISTDIR)/src/richtext
	mkdir $(DISTDIR)/src/$(TOOLKITDIR)
	mkdir $(DISTDIR)/src/png
	mkdir $(DISTDIR)/src/jpeg
	mkdir $(DISTDIR)/src/tiff
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/xrc
	$(LN_SF) $(SRCDIR)/xrc/*.cpp $(DISTDIR)/src/xrc
	-$(LN_SF) $(COMMDIR)/*.cpp $(DISTDIR)/src/common
	$(LN_SF) $(COMMDIR)/*.c $(DISTDIR)/src/common
	$(LN_SF) $(COMMDIR)/*.inc $(DISTDIR)/src/common
	$(LN_SF) $(COMMDIR)/*.mms $(DISTDIR)/src/common
	$(LN_SF) $(UNIXDIR)/*.cpp $(DISTDIR)/src/unix
	$(LN_SF) $(UNIXDIR)/*.mms $(DISTDIR)/src/unix
	$(LN_SF) $(GENDIR)/*.cpp $(DISTDIR)/src/generic
	$(LN_SF) $(GENDIR)/*.mms $(DISTDIR)/src/generic
	$(LN_SF) $(HTMLDIR)/*.cpp $(DISTDIR)/src/html
	$(LN_SF) $(RICHTEXTDIR)/*.cpp $(DISTDIR)/src/richtext
	$(LN_SF) $(PNGDIR)/*.h $(DISTDIR)/src/png
	$(LN_SF) $(PNGDIR)/*.c $(DISTDIR)/src/png
	$(LN_SF) $(PNGDIR)/README $(DISTDIR)/src/png
	$(LN_SF) $(JPEGDIR)/*.h $(DISTDIR)/src/jpeg
	$(LN_SF) $(JPEGDIR)/*.c $(DISTDIR)/src/jpeg
	$(LN_SF) $(JPEGDIR)/README $(DISTDIR)/src/jpeg
	$(LN_SF) $(TIFFDIR)/*.h $(DISTDIR)/src/tiff
	$(LN_SF) $(TIFFDIR)/*.c $(DISTDIR)/src/tiff
	$(LN_SF) $(TIFFDIR)/README $(DISTDIR)/src/tiff

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
	mkdir $(DISTDIR)/include/wx/richtext
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/msdos
	mkdir $(DISTDIR)/src/msw
	$(LN_SF) $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	$(LN_SF) $(DOCDIR)/base/readme.txt $(DISTDIR)/README.txt
	$(LN_SF) $(WXDIR)/src/common/*.inc $(DISTDIR)/src/common
	$(LN_SF) $(WXDIR)/src/common/base.rc $(DISTDIR)/src/common
	list='$(ALL_PORTS_BASE_HEADERS)'; for p in $$list; do \
	  $(LN_SF) $(WXDIR)/include/$$p $(DISTDIR)/include/$$p; \
	done
	list='$(ALL_BASE_SOURCES)'; for p in $$list; do \
	  $(LN_SF) $(WXDIR)/$$p $(DISTDIR)/$$p; \
	done

	mkdir $(DISTDIR)/samples
	$(LN_SF) $(SAMPDIR)/Makefile.in $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/console.cpp $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/console.dsp $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

GTK_DIST: ALL_GUI_DIST
	$(LN_SF) $(WXDIR)/wxGTK.spec $(DISTDIR)
	$(LN_SF) $(INCDIR)/wx/gtk/*.h $(DISTDIR)/include/wx/gtk
	$(LN_SF) $(GTKDIR)/*.h $(DISTDIR)/src/gtk
	$(LN_SF) $(GTKDIR)/*.cpp $(DISTDIR)/src/gtk
	$(LN_SF) $(GTKDIR)/*.c $(DISTDIR)/src/gtk
	$(LN_SF) $(GTKDIR)/*.xbm $(DISTDIR)/src/gtk
	$(LN_SF) $(GTKDIR)/*.mms $(DISTDIR)/src/gtk

	mkdir $(DISTDIR)/include/wx/gtk/gnome
	mkdir $(DISTDIR)/src/gtk/gnome
	$(LN_SF) $(INCDIR)/wx/gtk/gnome/*.h $(DISTDIR)/include/wx/gtk/gnome
	$(LN_SF) $(GTKDIR)/gnome/*.cpp $(DISTDIR)/src/gtk/gnome

	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(LN_SF) $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(LN_SF) $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation

	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

X11_DIST: ALL_GUI_DIST UNIV_DIST
	$(LN_SF) $(WXDIR)/wxX11.spec $(DISTDIR)
	$(LN_SF) $(INCDIR)/wx/x11/*.h $(DISTDIR)/include/wx/x11
	$(LN_SF) $(X11DIR)/*.cpp $(DISTDIR)/src/x11
	$(LN_SF) $(X11DIR)/*.c $(DISTDIR)/src/x11
	$(LN_SF) $(X11DIR)/*.xbm $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(LN_SF) $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(LN_SF) $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

MOTIF_DIST: ALL_GUI_DIST
	$(LN_SF) $(WXDIR)/wxMotif.spec $(DISTDIR)
	$(LN_SF) $(INCDIR)/wx/motif/*.h $(DISTDIR)/include/wx/motif
	$(LN_SF) $(MOTIFDIR)/*.cpp $(DISTDIR)/src/motif
	$(LN_SF) $(MOTIFDIR)/*.xbm $(DISTDIR)/src/motif
	mkdir $(DISTDIR)/src/motif/xmcombo
	$(LN_SF) $(MOTIFDIR)/xmcombo/*.c $(DISTDIR)/src/motif/xmcombo
	$(LN_SF) $(MOTIFDIR)/xmcombo/*.h $(DISTDIR)/src/motif/xmcombo
	$(LN_SF) $(MOTIFDIR)/xmcombo/copying.txt $(DISTDIR)/src/motif/xmcombo
	mkdir $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/include/wx/x11
	$(LN_SF) $(X11DIR)/pen.cpp $(X11DIR)/brush.cpp $(X11DIR)/utilsx.cpp \
		$(X11DIR)/bitmap.cpp $(X11DIR)/glcanvas.cpp $(X11DIR)/region.cpp \
		$(DISTDIR)/src/x11
	$(LN_SF) $(X11INC)/pen.h $(X11INC)/brush.h $(X11INC)/privx.h \
		$(X11INC)/bitmap.h $(X11INC)/glcanvas.h $(X11INC)/private.h $(X11INC)/region.h \
		$(DISTDIR)/include/wx/x11
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

MACX_DIST: ALL_GUI_DIST
	$(LN_SF) $(INCDIR)/*.* $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx/mac/carbon
	mkdir $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/carbon/private
	$(LN_SF) $(INCDIR)/wx/mac/*.h $(DISTDIR)/include/wx/mac
	$(LN_SF) $(INCDIR)/wx/mac/carbon/*.h $(DISTDIR)/include/wx/mac/carbon
	$(LN_SF) $(INCDIR)/wx/mac/carbon/private/*.h $(DISTDIR)/include/wx/mac/carbon/private
	$(LN_SF) $(INCDIR)/wx/mac/private/*.h $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(LN_SF) $(INCDIR)/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(LN_SF) $(MACDIR)/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/src/html/htmlctrl
	mkdir $(DISTDIR)/src/html/htmlctrl/webkit
	$(LN_SF) $(WXDIR)/src/html/htmlctrl/webkit/*.mm $(DISTDIR)/src/html/htmlctrl/webkit
	mkdir $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/*.cpp $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/*.mm $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/*.icns $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/*.h $(DISTDIR)/src/mac/carbon
	$(LN_SF) $(MACDIR)/carbon/*.r $(DISTDIR)/src/mac/carbon
	mkdir $(DISTDIR)/src/mac/carbon/morefile
	$(LN_SF) $(MACDIR)/carbon/morefile/*.h $(DISTDIR)/src/mac/carbon/morefile
	$(LN_SF) $(MACDIR)/carbon/morefile/*.c $(DISTDIR)/src/mac/carbon/morefile
	mkdir $(DISTDIR)/src/mac/carbon/morefilex
	$(LN_SF) $(MACDIR)/carbon/morefilex/*.h $(DISTDIR)/src/mac/carbon/morefilex
	$(LN_SF) $(MACDIR)/carbon/morefilex/*.c $(DISTDIR)/src/mac/carbon/morefilex
	$(LN_SF) $(MACDIR)/carbon/morefilex/*.cpp $(DISTDIR)/src/mac/carbon/morefilex
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

# TODO: Distribute some files
COCOA_DIST: ALL_GUI_DIST
	$(LN_SF) $(COCOADIR)/*.mm $(DISTDIR)/src/cocoa

MSW_DIST: ALL_GUI_DIST
	$(LN_SF) $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	$(LN_SF) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.cur $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.ico $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.bmp $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/wx.manifest $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	$(LN_SF) $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	$(LN_SF) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(LN_SF) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(LN_SF) $(MSWDIR)/*.rc $(DISTDIR)/src/msw
	$(LN_SF) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole

MSW_ZIP_TEXT_DIST: ALL_GUI_DIST
	$(LN_SF) $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	$(LN_SF) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/wx.manifest $(DISTDIR)/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	$(LN_SF) $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	$(LN_SF) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(LN_SF) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(LN_SF) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	$(LN_SF) $(MSWDIR)/wince/*.* $(DISTDIR)/src/msw/wince
	$(LN_SF) $(SRCDIR)/*.??? $(DISTDIR)/src
	$(LN_SF) $(SRCDIR)/*.?? $(DISTDIR)/src
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

UNIV_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/include/wx/univ
	mkdir $(DISTDIR)/src/univ
	mkdir $(DISTDIR)/src/univ/themes
	$(LN_SF) $(INCDIR)/wx/univ/*.h $(DISTDIR)/include/wx/univ
	$(LN_SF) $(INCDIR)/wx/univ/setup0.h $(DISTDIR)/include/wx/univ/setup.h
	$(LN_SF) $(SRCDIR)/univ/*.cpp $(DISTDIR)/src/univ
	$(LN_SF) $(SRCDIR)/univ/themes/*.cpp $(DISTDIR)/src/univ/themes

MGL_DIST: UNIV_DIST
	$(LN_SF) $(WXDIR)/wxMGL.spec $(DISTDIR)
	$(LN_SF) $(INCDIR)/wx/mgl/*.h $(DISTDIR)/include/wx/mgl
	mkdir $(DISTDIR)/include/wx/msdos
	$(LN_SF) $(INCDIR)/wx/msdos/*.h $(DISTDIR)/include/wx/msdos
	$(LN_SF) $(SRCDIR)/mgl/make* $(DISTDIR)/src/mgl
	$(LN_SF) $(SRCDIR)/mgl/*.cpp $(DISTDIR)/src/mgl
	mkdir $(DISTDIR)/src/msdos
	$(LN_SF) $(SRCDIR)/msdos/*.cpp $(DISTDIR)/src/msdos
	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/contrib
	$(LNDIR) $(WXDIR)/contrib $(DISTDIR)/contrib

DEMOS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/demos
	$(LN_SF) $(DEMODIR)/Makefile.in $(DISTDIR)/demos

	mkdir $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/Makefile.in $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/makefile.unx $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/*.cpp $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/*.h $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/*.xpm $(DISTDIR)/demos/bombs
	$(LN_SF) $(DEMODIR)/bombs/readme.txt $(DISTDIR)/demos/bombs

	test "$(LNDIR)" != lndir || mkdir $(DISTDIR)/demos/dbbrowse
	$(LNDIR) $(DEMODIR)/dbbrowse $(DISTDIR)/demos/dbbrowse

	mkdir $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/Makefile.in $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/makefile.unx $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/*.h $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/*.cpp $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/*.xpm $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/*.xbm $(DISTDIR)/demos/forty
	$(LN_SF) $(DEMODIR)/forty/*.htm $(DISTDIR)/demos/forty

	mkdir $(DISTDIR)/demos/life
	mkdir $(DISTDIR)/demos/life/bitmaps
	$(LN_SF) $(DEMODIR)/life/Makefile.in $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/makefile.unx $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/*.cpp $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/*.h $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/*.xpm $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/*.inc $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/*.lif $(DISTDIR)/demos/life
	$(LN_SF) $(DEMODIR)/life/bitmaps/*.xpm $(DISTDIR)/demos/life/bitmaps

	mkdir $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/Makefile.in $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/makefile.unx $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/*.h $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/*.cpp $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/*.xpm $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/*.dat $(DISTDIR)/demos/poem
	$(LN_SF) $(DEMODIR)/poem/*.txt $(DISTDIR)/demos/poem

	mkdir $(DISTDIR)/demos/fractal
	$(LN_SF) $(DEMODIR)/fractal/Makefile.in $(DISTDIR)/demos/fractal
	$(LN_SF) $(DEMODIR)/fractal/makefile.unx $(DISTDIR)/demos/fractal
	$(LN_SF) $(DEMODIR)/fractal/*.cpp $(DISTDIR)/demos/fractal

SAMPLES_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/samples
	$(LN_SF) $(SAMPDIR)/Makefile.in $(DISTDIR)/samples
	$(LN_SF) $(SAMPDIR)/sample.* $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/artprov
	$(LN_SF) $(SAMPDIR)/artprov/Makefile.in $(DISTDIR)/samples/artprov
	$(LN_SF) $(SAMPDIR)/artprov/makefile.unx $(DISTDIR)/samples/artprov
	$(LN_SF) $(SAMPDIR)/artprov/*.cpp $(DISTDIR)/samples/artprov
	$(LN_SF) $(SAMPDIR)/artprov/*.h $(DISTDIR)/samples/artprov
	$(LN_SF) $(SAMPDIR)/artprov/*.xpm $(DISTDIR)/samples/artprov

	mkdir $(DISTDIR)/samples/calendar
	$(LN_SF) $(SAMPDIR)/calendar/Makefile.in $(DISTDIR)/samples/calendar
	$(LN_SF) $(SAMPDIR)/calendar/makefile.unx $(DISTDIR)/samples/calendar
	$(LN_SF) $(SAMPDIR)/calendar/*.cpp $(DISTDIR)/samples/calendar

	mkdir $(DISTDIR)/samples/caret
	$(LN_SF) $(SAMPDIR)/caret/Makefile.in $(DISTDIR)/samples/caret
	$(LN_SF) $(SAMPDIR)/caret/makefile.unx $(DISTDIR)/samples/caret
	$(LN_SF) $(SAMPDIR)/caret/*.cpp $(DISTDIR)/samples/caret
	$(LN_SF) $(SAMPDIR)/caret/*.xpm $(DISTDIR)/samples/caret

	mkdir $(DISTDIR)/samples/checklst
	$(LN_SF) $(SAMPDIR)/checklst/Makefile.in $(DISTDIR)/samples/checklst
	$(LN_SF) $(SAMPDIR)/checklst/makefile.unx $(DISTDIR)/samples/checklst
	$(LN_SF) $(SAMPDIR)/checklst/*.cpp $(DISTDIR)/samples/checklst
	$(LN_SF) $(SAMPDIR)/checklst/*.xpm $(DISTDIR)/samples/checklst
	$(LN_SF) $(SAMPDIR)/checklst/*.mms $(DISTDIR)/samples/checklst

	mkdir $(DISTDIR)/samples/config
	$(LN_SF) $(SAMPDIR)/config/Makefile.in $(DISTDIR)/samples/config
	$(LN_SF) $(SAMPDIR)/config/makefile.unx $(DISTDIR)/samples/config
	$(LN_SF) $(SAMPDIR)/config/*.cpp $(DISTDIR)/samples/config

	mkdir $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/*.cpp $(DISTDIR)/samples/console
	$(LN_SF) $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

	mkdir $(DISTDIR)/samples/controls
	mkdir $(DISTDIR)/samples/controls/icons
	$(LN_SF) $(SAMPDIR)/controls/Makefile.in $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/makefile.unx $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.cpp $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.xpm $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.bmp $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.ico $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.rc $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/*.mms $(DISTDIR)/samples/controls
	$(LN_SF) $(SAMPDIR)/controls/icons/*.??? $(DISTDIR)/samples/controls/icons

	mkdir $(DISTDIR)/samples/db
	$(LN_SF) $(SAMPDIR)/db/Makefile.in $(DISTDIR)/samples/db
	$(LN_SF) $(SAMPDIR)/db/makefile.unx $(DISTDIR)/samples/db
	$(LN_SF) $(SAMPDIR)/db/*.cpp $(DISTDIR)/samples/db
	$(LN_SF) $(SAMPDIR)/db/*.h $(DISTDIR)/samples/db
	$(LN_SF) $(SAMPDIR)/db/*.xpm $(DISTDIR)/samples/db

	mkdir $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/Makefile.in $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/makefile.unx $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/*.cpp $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/*.h $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/*.txt $(DISTDIR)/samples/dialogs
	$(LN_SF) $(SAMPDIR)/dialogs/*.mms $(DISTDIR)/samples/dialogs

	mkdir $(DISTDIR)/samples/dialup
	$(LN_SF) $(SAMPDIR)/dialup/Makefile.in $(DISTDIR)/samples/dialup
	$(LN_SF) $(SAMPDIR)/dialup/makefile.unx $(DISTDIR)/samples/dialup
	$(LN_SF) $(SAMPDIR)/dialup/*.cpp $(DISTDIR)/samples/dialup

	mkdir $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/Makefile.in $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/makefile.unx $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/*.cpp $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/*.xpm $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/*.txt $(DISTDIR)/samples/dnd
	$(LN_SF) $(SAMPDIR)/dnd/*.png $(DISTDIR)/samples/dnd

	mkdir $(DISTDIR)/samples/docview
	$(LN_SF) $(SAMPDIR)/docview/Makefile.in $(DISTDIR)/samples/docview
	$(LN_SF) $(SAMPDIR)/docview/makefile.unx $(DISTDIR)/samples/docview
	$(LN_SF) $(SAMPDIR)/docview/*.cpp $(DISTDIR)/samples/docview
	$(LN_SF) $(SAMPDIR)/docview/*.h $(DISTDIR)/samples/docview
	$(LN_SF) $(SAMPDIR)/docview/*.xpm $(DISTDIR)/samples/docview

	mkdir $(DISTDIR)/samples/docvwmdi
	$(LN_SF) $(SAMPDIR)/docvwmdi/Makefile.in $(DISTDIR)/samples/docvwmdi
	$(LN_SF) $(SAMPDIR)/docvwmdi/makefile.unx $(DISTDIR)/samples/docvwmdi
	$(LN_SF) $(SAMPDIR)/docvwmdi/*.cpp $(DISTDIR)/samples/docvwmdi
	$(LN_SF) $(SAMPDIR)/docvwmdi/*.h $(DISTDIR)/samples/docvwmdi

	mkdir $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/Makefile.in $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/makefile.unx $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/*.cpp $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/*.h $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/*.xpm $(DISTDIR)/samples/dragimag
	$(LN_SF) $(SAMPDIR)/dragimag/*.png $(DISTDIR)/samples/dragimag

	mkdir $(DISTDIR)/samples/drawing
	$(LN_SF) $(SAMPDIR)/drawing/Makefile.in $(DISTDIR)/samples/drawing
	$(LN_SF) $(SAMPDIR)/drawing/makefile.unx $(DISTDIR)/samples/drawing
	$(LN_SF) $(SAMPDIR)/drawing/*.cpp $(DISTDIR)/samples/drawing
	$(LN_SF) $(SAMPDIR)/drawing/*.xpm $(DISTDIR)/samples/drawing
	$(LN_SF) $(SAMPDIR)/drawing/*.bmp $(DISTDIR)/samples/drawing

	mkdir $(DISTDIR)/samples/dynamic
	$(LN_SF) $(SAMPDIR)/dynamic/Makefile.in $(DISTDIR)/samples/dynamic
	$(LN_SF) $(SAMPDIR)/dynamic/makefile.unx $(DISTDIR)/samples/dynamic
	$(LN_SF) $(SAMPDIR)/dynamic/*.cpp $(DISTDIR)/samples/dynamic
	$(LN_SF) $(SAMPDIR)/dynamic/*.xpm $(DISTDIR)/samples/dynamic

	mkdir $(DISTDIR)/samples/erase
	$(LN_SF) $(SAMPDIR)/erase/Makefile.in $(DISTDIR)/samples/erase
	$(LN_SF) $(SAMPDIR)/erase/makefile.unx $(DISTDIR)/samples/erase
	$(LN_SF) $(SAMPDIR)/erase/*.cpp $(DISTDIR)/samples/erase
	$(LN_SF) $(SAMPDIR)/erase/*.xml $(DISTDIR)/samples/erase

	mkdir $(DISTDIR)/samples/event
	$(LN_SF) $(SAMPDIR)/event/Makefile.in $(DISTDIR)/samples/event
	$(LN_SF) $(SAMPDIR)/event/makefile.unx $(DISTDIR)/samples/event
	$(LN_SF) $(SAMPDIR)/event/*.cpp $(DISTDIR)/samples/event

	mkdir $(DISTDIR)/samples/except
	$(LN_SF) $(SAMPDIR)/except/Makefile.in $(DISTDIR)/samples/except
	$(LN_SF) $(SAMPDIR)/except/*.cpp $(DISTDIR)/samples/except

	mkdir $(DISTDIR)/samples/exec
	$(LN_SF) $(SAMPDIR)/exec/Makefile.in $(DISTDIR)/samples/exec
	$(LN_SF) $(SAMPDIR)/exec/makefile.unx $(DISTDIR)/samples/exec
	$(LN_SF) $(SAMPDIR)/exec/*.cpp $(DISTDIR)/samples/exec

	mkdir $(DISTDIR)/samples/font
	$(LN_SF) $(SAMPDIR)/font/Makefile.in $(DISTDIR)/samples/font
	$(LN_SF) $(SAMPDIR)/font/makefile.unx $(DISTDIR)/samples/font
	$(LN_SF) $(SAMPDIR)/font/*.cpp $(DISTDIR)/samples/font

	mkdir $(DISTDIR)/samples/grid
	$(LN_SF) $(SAMPDIR)/grid/Makefile.in $(DISTDIR)/samples/grid
	$(LN_SF) $(SAMPDIR)/grid/makefile.unx $(DISTDIR)/samples/grid
	$(LN_SF) $(SAMPDIR)/grid/*.cpp $(SAMPDIR)/grid/*.h $(DISTDIR)/samples/grid

	mkdir $(DISTDIR)/samples/help
	mkdir $(DISTDIR)/samples/help/doc
	$(LN_SF) $(SAMPDIR)/help/Makefile.in $(DISTDIR)/samples/help
	$(LN_SF) $(SAMPDIR)/help/makefile.unx $(DISTDIR)/samples/help
	$(LN_SF) $(SAMPDIR)/help/*.cpp $(DISTDIR)/samples/help
	$(LN_SF) $(SAMPDIR)/help/*.xpm $(DISTDIR)/samples/help
	$(LN_SF) $(SAMPDIR)/help/doc/*.* $(DISTDIR)/samples/help/doc

	mkdir $(DISTDIR)/samples/htlbox
	$(LN_SF) $(SAMPDIR)/htlbox/Makefile.in $(DISTDIR)/samples/htlbox
	$(LN_SF) $(SAMPDIR)/htlbox/makefile.unx $(DISTDIR)/samples/htlbox
	$(LN_SF) $(SAMPDIR)/htlbox/*.cpp $(DISTDIR)/samples/htlbox
	$(LN_SF) $(SAMPDIR)/htlbox/*.xpm $(DISTDIR)/samples/htlbox

	mkdir $(DISTDIR)/samples/html
	$(LN_SF) $(SAMPDIR)/html/Makefile.in $(DISTDIR)/samples/html
	mkdir $(DISTDIR)/samples/html/about
	$(LN_SF) $(SAMPDIR)/html/about/Makefile.in $(DISTDIR)/samples/html/about
	$(LN_SF) $(SAMPDIR)/html/about/*.cpp $(DISTDIR)/samples/html/about
	mkdir $(DISTDIR)/samples/html/about/data
	$(LN_SF) $(SAMPDIR)/html/about/data/*.htm $(DISTDIR)/samples/html/about/data
	$(LN_SF) $(SAMPDIR)/html/about/data/*.png $(DISTDIR)/samples/html/about/data
	mkdir $(DISTDIR)/samples/html/help
	$(LN_SF) $(SAMPDIR)/html/help/Makefile.in $(DISTDIR)/samples/html/help
	$(LN_SF) $(SAMPDIR)/html/help/*.cpp $(DISTDIR)/samples/html/help
	mkdir $(DISTDIR)/samples/html/help/helpfiles
	$(LN_SF) $(SAMPDIR)/html/help/helpfiles/*.??? $(DISTDIR)/samples/html/help/helpfiles
	mkdir $(DISTDIR)/samples/html/helpview
	$(LN_SF) $(SAMPDIR)/html/helpview/Makefile.in $(DISTDIR)/samples/html/helpview
	$(LN_SF) $(SAMPDIR)/html/helpview/*.cpp $(DISTDIR)/samples/html/helpview
	$(LN_SF) $(SAMPDIR)/html/helpview/*.zip $(DISTDIR)/samples/html/helpview
	mkdir $(DISTDIR)/samples/html/printing
	$(LN_SF) $(SAMPDIR)/html/printing/Makefile.in $(DISTDIR)/samples/html/printing
	$(LN_SF) $(SAMPDIR)/html/printing/*.cpp $(DISTDIR)/samples/html/printing
	$(LN_SF) $(SAMPDIR)/html/printing/*.htm $(DISTDIR)/samples/html/printing
	mkdir $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/Makefile.in $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.cpp $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.bmp $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.png $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.gif $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.htm $(DISTDIR)/samples/html/test
	$(LN_SF) $(SAMPDIR)/html/test/*.html $(DISTDIR)/samples/html/test
	mkdir $(DISTDIR)/samples/html/virtual
	$(LN_SF) $(SAMPDIR)/html/virtual/Makefile.in $(DISTDIR)/samples/html/virtual
	$(LN_SF) $(SAMPDIR)/html/virtual/*.cpp $(DISTDIR)/samples/html/virtual
	$(LN_SF) $(SAMPDIR)/html/virtual/*.htm $(DISTDIR)/samples/html/virtual
	mkdir $(DISTDIR)/samples/html/widget
	$(LN_SF) $(SAMPDIR)/html/widget/Makefile.in $(DISTDIR)/samples/html/widget
	$(LN_SF) $(SAMPDIR)/html/widget/*.cpp $(DISTDIR)/samples/html/widget
	$(LN_SF) $(SAMPDIR)/html/widget/*.htm $(DISTDIR)/samples/html/widget
	mkdir $(DISTDIR)/samples/html/zip
	$(LN_SF) $(SAMPDIR)/html/zip/Makefile.in $(DISTDIR)/samples/html/zip
	$(LN_SF) $(SAMPDIR)/html/zip/*.cpp $(DISTDIR)/samples/html/zip
	$(LN_SF) $(SAMPDIR)/html/zip/*.htm $(DISTDIR)/samples/html/zip
	$(LN_SF) $(SAMPDIR)/html/zip/*.zip $(DISTDIR)/samples/html/zip

	mkdir $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/Makefile.in $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/makefile.unx $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/*.cpp $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/*.mms $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/horse*.* $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/smile.xbm $(DISTDIR)/samples/image
	$(LN_SF) $(SAMPDIR)/image/smile.xpm $(DISTDIR)/samples/image

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
	$(LN_SF) $(SAMPDIR)/internat/Makefile.in $(DISTDIR)/samples/internat
	$(LN_SF) $(SAMPDIR)/internat/makefile.unx $(DISTDIR)/samples/internat
	$(LN_SF) $(SAMPDIR)/internat/*.cpp $(DISTDIR)/samples/internat
	$(LN_SF) $(SAMPDIR)/internat/*.xpm $(DISTDIR)/samples/internat
	$(LN_SF) $(SAMPDIR)/internat/*.txt $(DISTDIR)/samples/internat
	$(LN_SF) $(SAMPDIR)/internat/bg/*.mo $(DISTDIR)/samples/internat/bg
	$(LN_SF) $(SAMPDIR)/internat/cs/*.mo $(DISTDIR)/samples/internat/cs
	$(LN_SF) $(SAMPDIR)/internat/de/*.mo $(DISTDIR)/samples/internat/de
	$(LN_SF) $(SAMPDIR)/internat/fr/*.mo $(DISTDIR)/samples/internat/fr
	$(LN_SF) $(SAMPDIR)/internat/ja/*.mo $(DISTDIR)/samples/internat/ja
	$(LN_SF) $(SAMPDIR)/internat/ka/*.mo $(DISTDIR)/samples/internat/ka
	$(LN_SF) $(SAMPDIR)/internat/pl/*.mo $(DISTDIR)/samples/internat/pl
	$(LN_SF) $(SAMPDIR)/internat/ru/*.mo $(DISTDIR)/samples/internat/ru
	$(LN_SF) $(SAMPDIR)/internat/sv/*.mo $(DISTDIR)/samples/internat/sv
	$(LN_SF) $(SAMPDIR)/internat/bg/*.po $(DISTDIR)/samples/internat/bg
	$(LN_SF) $(SAMPDIR)/internat/cs/*.po $(DISTDIR)/samples/internat/cs
	$(LN_SF) $(SAMPDIR)/internat/de/*.po $(DISTDIR)/samples/internat/de
	$(LN_SF) $(SAMPDIR)/internat/fr/*.po $(DISTDIR)/samples/internat/fr
	$(LN_SF) $(SAMPDIR)/internat/ja/*.po $(DISTDIR)/samples/internat/ja
	$(LN_SF) $(SAMPDIR)/internat/ka/*.po $(DISTDIR)/samples/internat/ka
	$(LN_SF) $(SAMPDIR)/internat/pl/*.po $(DISTDIR)/samples/internat/pl
	$(LN_SF) $(SAMPDIR)/internat/ru/*.po $(DISTDIR)/samples/internat/ru
	$(LN_SF) $(SAMPDIR)/internat/sv/*.po $(DISTDIR)/samples/internat/sv

	mkdir $(DISTDIR)/samples/ipc
	$(LN_SF) $(SAMPDIR)/ipc/Makefile.in $(DISTDIR)/samples/ipc
	$(LN_SF) $(SAMPDIR)/ipc/makefile.unx $(DISTDIR)/samples/ipc
	$(LN_SF) $(SAMPDIR)/ipc/*.cpp $(DISTDIR)/samples/ipc
	$(LN_SF) $(SAMPDIR)/ipc/*.h $(DISTDIR)/samples/ipc
	$(LN_SF) $(SAMPDIR)/ipc/*.xpm $(DISTDIR)/samples/ipc

	mkdir $(DISTDIR)/samples/joytest
	$(LN_SF) $(SAMPDIR)/joytest/Makefile.in $(DISTDIR)/samples/joytest
	$(LN_SF) $(SAMPDIR)/joytest/makefile.unx $(DISTDIR)/samples/joytest
	$(LN_SF) $(SAMPDIR)/joytest/*.cpp $(DISTDIR)/samples/joytest
	$(LN_SF) $(SAMPDIR)/joytest/*.h $(DISTDIR)/samples/joytest
	$(LN_SF) $(SAMPDIR)/joytest/*.wav $(DISTDIR)/samples/joytest

	mkdir $(DISTDIR)/samples/keyboard
	$(LN_SF) $(SAMPDIR)/keyboard/Makefile.in $(DISTDIR)/samples/keyboard
	$(LN_SF) $(SAMPDIR)/keyboard/*.cpp $(DISTDIR)/samples/keyboard

	mkdir $(DISTDIR)/samples/layout
	$(LN_SF) $(SAMPDIR)/layout/Makefile.in $(DISTDIR)/samples/layout
	$(LN_SF) $(SAMPDIR)/layout/makefile.unx $(DISTDIR)/samples/layout
	$(LN_SF) $(SAMPDIR)/layout/*.cpp $(DISTDIR)/samples/layout
	$(LN_SF) $(SAMPDIR)/layout/*.h $(DISTDIR)/samples/layout

	mkdir $(DISTDIR)/samples/listctrl
	mkdir $(DISTDIR)/samples/listctrl/bitmaps
	$(LN_SF) $(SAMPDIR)/listctrl/Makefile.in $(DISTDIR)/samples/listctrl
	$(LN_SF) $(SAMPDIR)/listctrl/makefile.unx $(DISTDIR)/samples/listctrl
	$(LN_SF) $(SAMPDIR)/listctrl/*.cpp $(DISTDIR)/samples/listctrl
	$(LN_SF) $(SAMPDIR)/listctrl/*.h $(DISTDIR)/samples/listctrl
	$(LN_SF) $(SAMPDIR)/listctrl/*.xpm $(DISTDIR)/samples/listctrl
	$(LN_SF) $(SAMPDIR)/listctrl/bitmaps/*.xpm $(DISTDIR)/samples/listctrl/bitmaps

	mkdir $(DISTDIR)/samples/mediaplayer
	$(LN_SF) $(SAMPDIR)/mediaplayer/Makefile.in $(DISTDIR)/samples/mediaplayer
	$(LN_SF) $(SAMPDIR)/mediaplayer/*.cpp $(DISTDIR)/samples/mediaplayer

	mkdir $(DISTDIR)/samples/mdi
	mkdir $(DISTDIR)/samples/mdi/bitmaps
	$(LN_SF) $(SAMPDIR)/mdi/Makefile.in $(DISTDIR)/samples/mdi
	$(LN_SF) $(SAMPDIR)/mdi/makefile.unx $(DISTDIR)/samples/mdi
	$(LN_SF) $(SAMPDIR)/mdi/*.cpp $(DISTDIR)/samples/mdi
	$(LN_SF) $(SAMPDIR)/mdi/*.h $(DISTDIR)/samples/mdi
	$(LN_SF) $(SAMPDIR)/mdi/*.xpm $(DISTDIR)/samples/mdi
	$(LN_SF) $(SAMPDIR)/mdi/bitmaps/*.xpm $(DISTDIR)/samples/mdi/bitmaps

	mkdir $(DISTDIR)/samples/memcheck
	$(LN_SF) $(SAMPDIR)/memcheck/Makefile.in $(DISTDIR)/samples/memcheck
	$(LN_SF) $(SAMPDIR)/memcheck/makefile.unx $(DISTDIR)/samples/memcheck
	$(LN_SF) $(SAMPDIR)/memcheck/*.cpp $(DISTDIR)/samples/memcheck
	$(LN_SF) $(SAMPDIR)/memcheck/*.xpm $(DISTDIR)/samples/memcheck

	mkdir $(DISTDIR)/samples/menu
	$(LN_SF) $(SAMPDIR)/menu/Makefile.in $(DISTDIR)/samples/menu
	$(LN_SF) $(SAMPDIR)/menu/makefile.unx $(DISTDIR)/samples/menu
	$(LN_SF) $(SAMPDIR)/menu/*.cpp $(DISTDIR)/samples/menu
	$(LN_SF) $(SAMPDIR)/menu/*.xpm $(DISTDIR)/samples/menu

	mkdir $(DISTDIR)/samples/minifram
	mkdir $(DISTDIR)/samples/minifram/bitmaps
	$(LN_SF) $(SAMPDIR)/minifram/Makefile.in $(DISTDIR)/samples/minifram
	$(LN_SF) $(SAMPDIR)/minifram/makefile.unx $(DISTDIR)/samples/minifram
	$(LN_SF) $(SAMPDIR)/minifram/*.cpp $(DISTDIR)/samples/minifram
	$(LN_SF) $(SAMPDIR)/minifram/*.h $(DISTDIR)/samples/minifram
	$(LN_SF) $(SAMPDIR)/minifram/*.xpm $(DISTDIR)/samples/minifram
	$(LN_SF) $(SAMPDIR)/minifram/bitmaps/*.xpm $(DISTDIR)/samples/minifram/bitmaps

	mkdir $(DISTDIR)/samples/minimal
	$(LN_SF) $(SAMPDIR)/minimal/Makefile.in $(DISTDIR)/samples/minimal
	$(LN_SF) $(SAMPDIR)/minimal/makefile.unx $(DISTDIR)/samples/minimal
	$(LN_SF) $(SAMPDIR)/minimal/*.cpp $(DISTDIR)/samples/minimal
	$(LN_SF) $(SAMPDIR)/minimal/*.mms $(DISTDIR)/samples/minimal

	mkdir $(DISTDIR)/samples/mobile
	$(LN_SF) $(SAMPDIR)/mobile/Makefile.in $(DISTDIR)/samples/mobile
	mkdir $(DISTDIR)/samples/mobile/wxedit
	$(LN_SF) $(SAMPDIR)/mobile/wxedit/Makefile.in $(DISTDIR)/samples/mobile/wxedit
	$(LN_SF) $(SAMPDIR)/mobile/wxedit/*.cpp $(DISTDIR)/samples/mobile/wxedit
	$(LN_SF) $(SAMPDIR)/mobile/wxedit/*.h $(DISTDIR)/samples/mobile/wxedit
	mkdir $(DISTDIR)/samples/mobile/styles
	$(LN_SF) $(SAMPDIR)/mobile/styles/Makefile.in $(DISTDIR)/samples/mobile/styles
	$(LN_SF) $(SAMPDIR)/mobile/styles/*.cpp $(DISTDIR)/samples/mobile/styles
	$(LN_SF) $(SAMPDIR)/mobile/styles/*.h $(DISTDIR)/samples/mobile/styles
	$(LN_SF) $(SAMPDIR)/mobile/styles/*.jpg $(DISTDIR)/samples/mobile/styles

	mkdir $(DISTDIR)/samples/multimon
	$(LN_SF) $(SAMPDIR)/multimon/Makefile.in $(DISTDIR)/samples/multimon
	$(LN_SF) $(SAMPDIR)/multimon/makefile.unx $(DISTDIR)/samples/multimon
	$(LN_SF) $(SAMPDIR)/multimon/*.cpp $(DISTDIR)/samples/multimon

	mkdir $(DISTDIR)/samples/notebook
	$(LN_SF) $(SAMPDIR)/notebook/Makefile.in $(DISTDIR)/samples/notebook
	$(LN_SF) $(SAMPDIR)/notebook/makefile.unx $(DISTDIR)/samples/notebook
	$(LN_SF) $(SAMPDIR)/notebook/*.cpp $(DISTDIR)/samples/notebook
	$(LN_SF) $(SAMPDIR)/notebook/*.h $(DISTDIR)/samples/notebook

	mkdir $(DISTDIR)/samples/opengl
	$(LN_SF) $(SAMPDIR)/opengl/Makefile.in $(DISTDIR)/samples/opengl
	mkdir $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/Makefile.in $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/makefile.unx $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/*.cpp $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/*.c $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/*.h $(DISTDIR)/samples/opengl/penguin
	$(LN_SF) $(SAMPDIR)/opengl/penguin/*.lwo $(DISTDIR)/samples/opengl/penguin
	mkdir $(DISTDIR)/samples/opengl/cube
	$(LN_SF) $(SAMPDIR)/opengl/cube/Makefile.in $(DISTDIR)/samples/opengl/cube
	$(LN_SF) $(SAMPDIR)/opengl/cube/makefile.unx $(DISTDIR)/samples/opengl/cube
	$(LN_SF) $(SAMPDIR)/opengl/cube/*.cpp $(DISTDIR)/samples/opengl/cube
	$(LN_SF) $(SAMPDIR)/opengl/cube/*.h $(DISTDIR)/samples/opengl/cube
	mkdir $(DISTDIR)/samples/opengl/isosurf
	$(LN_SF) $(SAMPDIR)/opengl/isosurf/Makefile.in $(DISTDIR)/samples/opengl/isosurf
	$(LN_SF) $(SAMPDIR)/opengl/isosurf/makefile.unx $(DISTDIR)/samples/opengl/isosurf
	$(LN_SF) $(SAMPDIR)/opengl/isosurf/*.cpp $(DISTDIR)/samples/opengl/isosurf
	$(LN_SF) $(SAMPDIR)/opengl/isosurf/*.h $(DISTDIR)/samples/opengl/isosurf
	$(LN_SF) $(SAMPDIR)/opengl/isosurf/*.gz $(DISTDIR)/samples/opengl/isosurf

	mkdir $(DISTDIR)/samples/png
	$(LN_SF) $(SAMPDIR)/png/Makefile.in $(DISTDIR)/samples/png
	$(LN_SF) $(SAMPDIR)/png/makefile.unx $(DISTDIR)/samples/png
	$(LN_SF) $(SAMPDIR)/png/*.cpp $(DISTDIR)/samples/png
	$(LN_SF) $(SAMPDIR)/png/*.h $(DISTDIR)/samples/png
	$(LN_SF) $(SAMPDIR)/png/*.png $(DISTDIR)/samples/png

	mkdir $(DISTDIR)/samples/popup
	$(LN_SF) $(SAMPDIR)/popup/Makefile.in $(DISTDIR)/samples/popup
	$(LN_SF) $(SAMPDIR)/popup/makefile.unx $(DISTDIR)/samples/popup
	$(LN_SF) $(SAMPDIR)/popup/*.cpp $(DISTDIR)/samples/popup

	mkdir $(DISTDIR)/samples/printing
	$(LN_SF) $(SAMPDIR)/printing/Makefile.in $(DISTDIR)/samples/printing
	$(LN_SF) $(SAMPDIR)/printing/makefile.unx $(DISTDIR)/samples/printing
	$(LN_SF) $(SAMPDIR)/printing/*.cpp $(DISTDIR)/samples/printing
	$(LN_SF) $(SAMPDIR)/printing/*.h $(DISTDIR)/samples/printing
	$(LN_SF) $(SAMPDIR)/printing/*.xpm $(DISTDIR)/samples/printing

	mkdir $(DISTDIR)/samples/render
	$(LN_SF) $(SAMPDIR)/render/Makefile.in $(DISTDIR)/samples/render
	$(LN_SF) $(SAMPDIR)/render/makefile.unx $(DISTDIR)/samples/render
	$(LN_SF) $(SAMPDIR)/render/*.cpp $(DISTDIR)/samples/render

	mkdir $(DISTDIR)/samples/rotate
	$(LN_SF) $(SAMPDIR)/rotate/Makefile.in $(DISTDIR)/samples/rotate
	$(LN_SF) $(SAMPDIR)/rotate/makefile.unx $(DISTDIR)/samples/rotate
	$(LN_SF) $(SAMPDIR)/rotate/*.cpp $(DISTDIR)/samples/rotate
	$(LN_SF) $(SAMPDIR)/rotate/*.png $(DISTDIR)/samples/rotate

	mkdir $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/Makefile.in $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/makefile.unx $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/*.cpp $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/*.h $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/*.xpm $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/README $(DISTDIR)/samples/richedit
	$(LN_SF) $(SAMPDIR)/richedit/TODO $(DISTDIR)/samples/richedit

	mkdir $(DISTDIR)/samples/richtext
	mkdir $(DISTDIR)/samples/richtext/bitmaps
	$(LN_SF) $(SAMPDIR)/richtext/Makefile.in $(DISTDIR)/samples/richtext
	$(LN_SF) $(SAMPDIR)/richtext/makefile.unx $(DISTDIR)/samples/richtext
	$(LN_SF) $(SAMPDIR)/richtext/*.cpp $(DISTDIR)/samples/richtext
	$(LN_SF) $(SAMPDIR)/richtext/*.h $(DISTDIR)/samples/richtext
	$(LN_SF) $(SAMPDIR)/richtext/bitmaps/*.xpm $(DISTDIR)/samples/richtext/bitmaps
	$(LN_SF) $(SAMPDIR)/richtext/readme.txt $(DISTDIR)/samples/richtext
	$(LN_SF) $(SAMPDIR)/richtext/todo.txt $(DISTDIR)/samples/richtext

	mkdir $(DISTDIR)/samples/propsize
	$(LN_SF) $(SAMPDIR)/propsize/Makefile.in $(DISTDIR)/samples/propsize
	$(LN_SF) $(SAMPDIR)/propsize/makefile.unx $(DISTDIR)/samples/propsize
	$(LN_SF) $(SAMPDIR)/propsize/*.cpp $(DISTDIR)/samples/propsize
	$(LN_SF) $(SAMPDIR)/propsize/*.xpm $(DISTDIR)/samples/propsize

	mkdir $(DISTDIR)/samples/sashtest
	$(LN_SF) $(SAMPDIR)/sashtest/Makefile.in $(DISTDIR)/samples/sashtest
	$(LN_SF) $(SAMPDIR)/sashtest/makefile.unx $(DISTDIR)/samples/sashtest
	$(LN_SF) $(SAMPDIR)/sashtest/*.cpp $(DISTDIR)/samples/sashtest
	$(LN_SF) $(SAMPDIR)/sashtest/*.h $(DISTDIR)/samples/sashtest

	mkdir $(DISTDIR)/samples/scroll
	$(LN_SF) $(SAMPDIR)/scroll/Makefile.in $(DISTDIR)/samples/scroll
	$(LN_SF) $(SAMPDIR)/scroll/makefile.unx $(DISTDIR)/samples/scroll
	$(LN_SF) $(SAMPDIR)/scroll/*.cpp $(DISTDIR)/samples/scroll

	mkdir $(DISTDIR)/samples/scrollsub
	$(LN_SF) $(SAMPDIR)/scrollsub/Makefile.in $(DISTDIR)/samples/scrollsub
	$(LN_SF) $(SAMPDIR)/scrollsub/makefile.unx $(DISTDIR)/samples/scrollsub
	$(LN_SF) $(SAMPDIR)/scrollsub/*.cpp $(DISTDIR)/samples/scrollsub

	mkdir $(DISTDIR)/samples/shaped
	$(LN_SF) $(SAMPDIR)/shaped/Makefile.in $(DISTDIR)/samples/shaped
	$(LN_SF) $(SAMPDIR)/shaped/makefile.unx $(DISTDIR)/samples/shaped
	$(LN_SF) $(SAMPDIR)/shaped/*.cpp $(DISTDIR)/samples/shaped
	$(LN_SF) $(SAMPDIR)/shaped/*.png $(DISTDIR)/samples/shaped

	mkdir $(DISTDIR)/samples/sockets
	$(LN_SF) $(SAMPDIR)/sockets/Makefile.in $(DISTDIR)/samples/sockets
	$(LN_SF) $(SAMPDIR)/sockets/makefile.unx $(DISTDIR)/samples/sockets
	$(LN_SF) $(SAMPDIR)/sockets/*.cpp $(DISTDIR)/samples/sockets
	$(LN_SF) $(SAMPDIR)/sockets/*.xpm $(DISTDIR)/samples/sockets

	mkdir $(DISTDIR)/samples/sound
	$(LN_SF) $(SAMPDIR)/sound/Makefile.in $(DISTDIR)/samples/sound
	$(LN_SF) $(SAMPDIR)/sound/*.cpp $(DISTDIR)/samples/sound
	$(LN_SF) $(SAMPDIR)/sound/*.wav $(DISTDIR)/samples/sound

	mkdir $(DISTDIR)/samples/splash
	$(LN_SF) $(SAMPDIR)/splash/Makefile.in $(DISTDIR)/samples/splash
	$(LN_SF) $(SAMPDIR)/splash/*.cpp $(DISTDIR)/samples/splash
	$(LN_SF) $(SAMPDIR)/splash/*.png $(DISTDIR)/samples/splash
	$(LN_SF) $(SAMPDIR)/splash/*.mpg $(DISTDIR)/samples/splash

	mkdir $(DISTDIR)/samples/splitter
	$(LN_SF) $(SAMPDIR)/splitter/Makefile.in $(DISTDIR)/samples/splitter
	$(LN_SF) $(SAMPDIR)/splitter/makefile.unx $(DISTDIR)/samples/splitter
	$(LN_SF) $(SAMPDIR)/splitter/*.cpp $(DISTDIR)/samples/splitter

	mkdir $(DISTDIR)/samples/statbar
	$(LN_SF) $(SAMPDIR)/statbar/Makefile.in $(DISTDIR)/samples/statbar
	$(LN_SF) $(SAMPDIR)/statbar/makefile.unx $(DISTDIR)/samples/statbar
	$(LN_SF) $(SAMPDIR)/statbar/*.cpp $(DISTDIR)/samples/statbar
	$(LN_SF) $(SAMPDIR)/statbar/*.xpm $(DISTDIR)/samples/statbar

	mkdir $(DISTDIR)/samples/text
	$(LN_SF) $(SAMPDIR)/text/Makefile.in $(DISTDIR)/samples/text
	$(LN_SF) $(SAMPDIR)/text/makefile.unx $(DISTDIR)/samples/text
	$(LN_SF) $(SAMPDIR)/text/*.cpp $(DISTDIR)/samples/text
	$(LN_SF) $(SAMPDIR)/text/*.xpm $(DISTDIR)/samples/text

	mkdir $(DISTDIR)/samples/thread
	$(LN_SF) $(SAMPDIR)/thread/Makefile.in $(DISTDIR)/samples/thread
	$(LN_SF) $(SAMPDIR)/thread/makefile.unx $(DISTDIR)/samples/thread
	$(LN_SF) $(SAMPDIR)/thread/*.cpp $(DISTDIR)/samples/thread

	mkdir $(DISTDIR)/samples/toolbar
	$(LN_SF) $(SAMPDIR)/toolbar/Makefile.in $(DISTDIR)/samples/toolbar
	$(LN_SF) $(SAMPDIR)/toolbar/makefile.unx $(DISTDIR)/samples/toolbar
	$(LN_SF) $(SAMPDIR)/toolbar/*.cpp $(DISTDIR)/samples/toolbar
	$(LN_SF) $(SAMPDIR)/toolbar/*.xpm $(DISTDIR)/samples/toolbar
	mkdir $(DISTDIR)/samples/toolbar/bitmaps
	$(LN_SF) $(SAMPDIR)/toolbar/bitmaps/*.xpm $(DISTDIR)/samples/toolbar/bitmaps

	mkdir $(DISTDIR)/samples/treectrl
	$(LN_SF) $(SAMPDIR)/treectrl/Makefile.in $(DISTDIR)/samples/treectrl
	$(LN_SF) $(SAMPDIR)/treectrl/makefile.unx $(DISTDIR)/samples/treectrl
	$(LN_SF) $(SAMPDIR)/treectrl/*.cpp $(DISTDIR)/samples/treectrl
	$(LN_SF) $(SAMPDIR)/treectrl/*.h $(DISTDIR)/samples/treectrl
	$(LN_SF) $(SAMPDIR)/treectrl/*.xpm $(DISTDIR)/samples/treectrl

	mkdir $(DISTDIR)/samples/typetest
	$(LN_SF) $(SAMPDIR)/typetest/Makefile.in $(DISTDIR)/samples/typetest
	$(LN_SF) $(SAMPDIR)/typetest/makefile.unx $(DISTDIR)/samples/typetest
	$(LN_SF) $(SAMPDIR)/typetest/*.cpp $(DISTDIR)/samples/typetest
	$(LN_SF) $(SAMPDIR)/typetest/*.h $(DISTDIR)/samples/typetest
	$(LN_SF) $(SAMPDIR)/typetest/*.xpm $(DISTDIR)/samples/typetest

	mkdir $(DISTDIR)/samples/validate
	$(LN_SF) $(SAMPDIR)/validate/Makefile.in $(DISTDIR)/samples/validate
	$(LN_SF) $(SAMPDIR)/validate/makefile.unx $(DISTDIR)/samples/validate
	$(LN_SF) $(SAMPDIR)/validate/*.cpp $(DISTDIR)/samples/validate
	$(LN_SF) $(SAMPDIR)/validate/*.h $(DISTDIR)/samples/validate
	$(LN_SF) $(SAMPDIR)/validate/*.xpm $(DISTDIR)/samples/validate

	mkdir $(DISTDIR)/samples/vscroll
	$(LN_SF) $(SAMPDIR)/vscroll/Makefile.in $(DISTDIR)/samples/vscroll
	$(LN_SF) $(SAMPDIR)/vscroll/*.cpp $(DISTDIR)/samples/vscroll

	mkdir $(DISTDIR)/samples/wizard
	$(LN_SF) $(SAMPDIR)/wizard/Makefile.in $(DISTDIR)/samples/wizard
	$(LN_SF) $(SAMPDIR)/wizard/makefile.unx $(DISTDIR)/samples/wizard
	$(LN_SF) $(SAMPDIR)/wizard/*.cpp $(DISTDIR)/samples/wizard
	$(LN_SF) $(SAMPDIR)/wizard/*.xpm $(DISTDIR)/samples/wizard

	mkdir $(DISTDIR)/samples/widgets
	mkdir $(DISTDIR)/samples/widgets/icons
	$(LN_SF) $(SAMPDIR)/widgets/Makefile.in $(DISTDIR)/samples/widgets
	$(LN_SF) $(SAMPDIR)/widgets/*.cpp $(DISTDIR)/samples/widgets
	$(LN_SF) $(SAMPDIR)/widgets/*.h $(DISTDIR)/samples/widgets
	$(LN_SF) $(SAMPDIR)/widgets/*.rc $(DISTDIR)/samples/widgets
	$(LN_SF) $(SAMPDIR)/widgets/icons/*.xpm $(DISTDIR)/samples/widgets/icons

	mkdir $(DISTDIR)/samples/xrc
	mkdir $(DISTDIR)/samples/xrc/rc
	$(LN_SF) $(SAMPDIR)/xrc/Makefile.in $(DISTDIR)/samples/xrc
	$(LN_SF) $(SAMPDIR)/xrc/*.cpp $(DISTDIR)/samples/xrc
	$(LN_SF) $(SAMPDIR)/xrc/*.h $(DISTDIR)/samples/xrc
	$(LN_SF) $(SAMPDIR)/xrc/*.rc $(DISTDIR)/samples/xrc
	$(LN_SF) $(SAMPDIR)/xrc/rc/*.xpm $(DISTDIR)/samples/xrc/rc
	$(LN_SF) $(SAMPDIR)/xrc/rc/*.xrc $(DISTDIR)/samples/xrc/rc
	$(LN_SF) $(SAMPDIR)/xrc/rc/*.gif $(DISTDIR)/samples/xrc/rc
	$(LN_SF) $(SAMPDIR)/xrc/rc/*.ico $(DISTDIR)/samples/xrc/rc

UTILS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/utils
	$(LN_SF) $(UTILSDIR)/Makefile.in $(DISTDIR)/utils

	mkdir $(DISTDIR)/utils/tex2rtf
	mkdir $(DISTDIR)/utils/tex2rtf/src
	$(LN_SF) $(UTILSDIR)/tex2rtf/*.in $(DISTDIR)/utils/tex2rtf
	$(LN_SF) $(UTILSDIR)/tex2rtf/src/*.h $(DISTDIR)/utils/tex2rtf/src
	$(LN_SF) $(UTILSDIR)/tex2rtf/src/*.in $(DISTDIR)/utils/tex2rtf/src
	$(LN_SF) $(UTILSDIR)/tex2rtf/src/*.cpp $(DISTDIR)/utils/tex2rtf/src
	-$(LN_SF) $(UTILSDIR)/tex2rtf/src/tex2rtf.* $(DISTDIR)/utils/tex2rtf/src

	mkdir $(DISTDIR)/utils/configtool
	mkdir $(DISTDIR)/utils/configtool/configs
	mkdir $(DISTDIR)/utils/configtool/docs
	mkdir $(DISTDIR)/utils/configtool/src
	mkdir $(DISTDIR)/utils/configtool/src/bitmaps
	mkdir $(DISTDIR)/utils/configtool/scripts
	$(LN_SF) $(UTILSDIR)/configtool/*.in $(DISTDIR)/utils/configtool
	$(LN_SF) $(UTILSDIR)/configtool/configs/*.wxs $(DISTDIR)/utils/configtool/configs
	$(LN_SF) $(UTILSDIR)/configtool/docs/*.txt $(DISTDIR)/utils/configtool/docs
	$(LN_SF) $(UTILSDIR)/configtool/src/*.h $(DISTDIR)/utils/configtool/src
	$(LN_SF) $(UTILSDIR)/configtool/src/*.in $(DISTDIR)/utils/configtool/src
	$(LN_SF) $(UTILSDIR)/configtool/src/*.cpp $(DISTDIR)/utils/configtool/src
	$(LN_SF) $(UTILSDIR)/configtool/src/bitmaps/*.xpm $(DISTDIR)/utils/configtool/src/bitmaps
	$(LN_SF) $(UTILSDIR)/configtool/scripts/* $(DISTDIR)/utils/configtool/scripts

	mkdir $(DISTDIR)/utils/emulator
	mkdir $(DISTDIR)/utils/emulator/src
	mkdir $(DISTDIR)/utils/emulator/docs
	$(LN_SF) $(UTILSDIR)/emulator/*.in $(DISTDIR)/utils/emulator
	$(LN_SF) $(UTILSDIR)/emulator/src/*.h $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/src/*.in $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/src/*.cpp $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/src/*.jpg $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/src/*.wxe $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/src/*.xpm $(DISTDIR)/utils/emulator/src
	$(LN_SF) $(UTILSDIR)/emulator/docs/*.txt $(DISTDIR)/utils/emulator/docs
	$(LN_SF) $(UTILSDIR)/emulator/docs/*.jpg $(DISTDIR)/utils/emulator/docs

	mkdir $(DISTDIR)/utils/hhp2cached
	$(LN_SF) $(UTILSDIR)/hhp2cached/Makefile.in $(DISTDIR)/utils/hhp2cached
	$(LN_SF) $(UTILSDIR)/hhp2cached/*.cpp $(DISTDIR)/utils/hhp2cached
	$(LN_SF) $(UTILSDIR)/hhp2cached/*.rc $(DISTDIR)/utils/hhp2cached

	mkdir $(DISTDIR)/utils/HelpGen
	mkdir $(DISTDIR)/utils/HelpGen/src
	$(LN_SF) $(UTILSDIR)/HelpGen/Makefile.in $(DISTDIR)/utils/HelpGen
	$(LN_SF) $(UTILSDIR)/HelpGen/src/Makefile.in $(DISTDIR)/utils/HelpGen/src
	$(LN_SF) $(UTILSDIR)/HelpGen/src/*.h $(DISTDIR)/utils/HelpGen/src
	$(LN_SF) $(UTILSDIR)/HelpGen/src/*.cpp $(DISTDIR)/utils/HelpGen/src

	mkdir $(DISTDIR)/utils/helpview
	mkdir $(DISTDIR)/utils/helpview/src
	mkdir $(DISTDIR)/utils/helpview/src/bitmaps
	$(LN_SF) $(UTILSDIR)/helpview/Makefile.in $(DISTDIR)/utils/helpview
	$(LN_SF) $(UTILSDIR)/helpview/src/*.h $(DISTDIR)/utils/helpview/src
	$(LN_SF) $(UTILSDIR)/helpview/src/*.cpp $(DISTDIR)/utils/helpview/src
	$(LN_SF) $(UTILSDIR)/helpview/src/Makefile.in $(DISTDIR)/utils/helpview/src
	$(LN_SF) $(UTILSDIR)/helpview/src/test.zip $(DISTDIR)/utils/helpview/src
	$(LN_SF) $(UTILSDIR)/helpview/src/bitmaps/*.xpm $(DISTDIR)/utils/helpview/src/bitmaps

	mkdir $(DISTDIR)/utils/wxrc
	$(LN_SF) $(UTILSDIR)/wxrc/Makefile.in $(DISTDIR)/utils/wxrc
	$(LN_SF) $(UTILSDIR)/wxrc/*.cpp $(DISTDIR)/utils/wxrc
	$(LN_SF) $(UTILSDIR)/wxrc/*.rc $(DISTDIR)/utils/wxrc

MISC_DIST: ALL_GUI_DIST

INTL_DIST:
	mkdir $(DISTDIR)/locale
	$(LN_SF) $(INTLDIR)/Makefile $(DISTDIR)/locale
	$(LN_SF) $(INTLDIR)/*.po $(DISTDIR)/locale
	-$(LN_SF) $(INTLDIR)/*.mo $(DISTDIR)/locale
	subdirs=`cd $(INTLDIR) && ls */*.po | sed 's|/.*||' | uniq`; \
	for dir in "$$subdirs"; do                                   \
	    mkdir $(DISTDIR)/locale/$$dir;                           \
	    $(LN_SF) $(INTLDIR)/$$dir/*.[pm]o $(DISTDIR)/locale/$$dir;     \
	done

MANUAL_DIST:
	mkdir $(DISTDIR)/docs
	mkdir $(DISTDIR)/docs/latex
	mkdir $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.tex $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.inc $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.gif $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.ini $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.bib $(DISTDIR)/docs/latex/wx
	$(LN_SF) $(DOCDIR)/latex/wx/*.sty $(DISTDIR)/docs/latex/wx


# Copy all the files from wxPython needed for the Debian source package,
# and then remove some that are not needed.
PYTHON_DIST:
	for dir in `grep -v '#' $(WXDIR)/wxPython/distrib/DIRLIST`; do \
		echo "Copying dir: $$dir..."; \
		mkdir $(DISTDIR)/$$dir; \
		$(LN_SF) $(WXDIR)/$$dir/* $(DISTDIR)/$$dir > /dev/null 2>&1; \
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
			-print | xargs rm -rf

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
	$(LN_SF) $(INCDIR)/wx/msw/*.cur _dist_dir/wxMSW/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.ico _dist_dir/wxMSW/include/wx/msw
	$(LN_SF) $(INCDIR)/wx/msw/*.bmp _dist_dir/wxMSW/include/wx/msw
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.cur
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.ico
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.bmp

debian-dist: DEBIAN_SOURCE_DIR = $(WXDIR)/../wxwidgets@WX_RELEASE@@WX_FLAVOUR@-@WX_SUBVERSION@
debian-dist: debian-native-dist debian-msw-dirs MSW_DIST
	mkdir $(DISTDIR)/debian
	-$(LN_SF) $(WXDIR)/debian/* $(DISTDIR)/debian
	$(LN_SF) $(DOCDIR)/licence.txt $(DISTDIR)/docs
	$(LN_SF) $(DOCDIR)/licendoc.txt $(DISTDIR)/docs
	$(LN_SF) $(DOCDIR)/preamble.txt $(DISTDIR)/docs
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
