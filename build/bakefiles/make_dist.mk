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
RICHTEXTDIR = $(WXDIR)/src/richtext
AUIDIR =   $(WXDIR)/src/aui
UNIXDIR  = $(WXDIR)/src/unix
PNGDIR   = $(WXDIR)/src/png
JPEGDIR  = $(WXDIR)/src/jpeg
TIFFDIR  = $(WXDIR)/src/tiff
ZLIBDIR  = $(WXDIR)/src/zlib
REGEXDIR = $(WXDIR)/src/regex
EXPATDIR = $(WXDIR)/src/expat
GTKDIR   = $(WXDIR)/src/gtk
GTK1DIR  = $(WXDIR)/src/gtk1
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

CP_PR = cp -pR
CP_P = cp -p

########################## make dist rules ###############################

# this target should copy only the files common to really all ports (including
# wxBase) to the dist dir
ALL_DIST: distrib_clean
	mkdir _dist_dir
	mkdir $(DISTDIR)
	$(CP_P) $(WXDIR)/configure.in $(DISTDIR)
	$(CP_P) $(WXDIR)/configure $(DISTDIR)
	$(CP_P) $(WXDIR)/autoconf_inc.m4 $(DISTDIR)
	$(CP_P) $(WXDIR)/wxwin.m4 $(DISTDIR)
	$(CP_P) $(WXDIR)/acinclude.m4 $(DISTDIR)
	$(CP_P) $(WXDIR)/aclocal.m4 $(DISTDIR)
	$(CP_P) $(WXDIR)/config.sub $(DISTDIR)
	$(CP_P) $(WXDIR)/config.guess $(DISTDIR)
	$(CP_P) $(WXDIR)/install-sh $(DISTDIR)
	$(CP_P) $(WXDIR)/mkinstalldirs $(DISTDIR)
	$(CP_P) $(WXDIR)/wx-config.in $(DISTDIR)
	$(CP_P) $(WXDIR)/wx-config-inplace.in $(DISTDIR)
	$(CP_P) $(WXDIR)/version-script.in $(DISTDIR)
	$(CP_P) $(WXDIR)/setup.h.in $(DISTDIR)
	$(CP_P) $(WXDIR)/setup.h_vms $(DISTDIR)
	$(CP_P) $(WXDIR)/descrip.mms $(DISTDIR)
	$(CP_P) $(WXDIR)/Makefile.in $(DISTDIR)
	$(CP_P) $(WXDIR)/wxBase.spec $(DISTDIR)
	$(CP_P) $(DOCDIR)/lgpl.txt $(DISTDIR)/COPYING.LIB
	$(CP_P) $(DOCDIR)/licence.txt $(DISTDIR)/LICENCE.txt
	$(CP_P) $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	mkdir $(DISTDIR)/lib
	$(CP_P) $(WXDIR)/lib/vms.opt $(DISTDIR)/lib
	$(CP_P) $(WXDIR)/lib/vms_gtk.opt $(DISTDIR)/lib
	mkdir $(DISTDIR)/src
	# temp hack for common/execcmn.cpp: it's not supported by tmake
	# yet (it's a header-like file but in src/common directory and it
	# shouldn't be distributed...)
	mkdir $(DISTDIR)/src/common
	$(CP_P) $(SRCDIR)/common/execcmn.cpp $(DISTDIR)/src/common
	mkdir $(DISTDIR)/src/xml
	$(CP_P) $(SRCDIR)/xml/*.cpp $(DISTDIR)/src/xml
	mkdir $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/*.h $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/*.c $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/README $(DISTDIR)/src/zlib
	#$(CP_P) $(ZLIBDIR)/*.mms $(DISTDIR)/src/zlib
	mkdir $(DISTDIR)/src/regex
	$(CP_P) $(REGEXDIR)/*.h $(DISTDIR)/src/regex
	$(CP_P) $(REGEXDIR)/*.c $(DISTDIR)/src/regex
	$(CP_P) $(REGEXDIR)/COPYRIGHT $(DISTDIR)/src/regex
	$(CP_P) $(REGEXDIR)/README $(DISTDIR)/src/regex
	$(CP_PR) $(EXPATDIR) $(DISTDIR)/src/expat
	#(cd $(DISTDIR)/src/expat ; rm -rf `find -name CVS`)
	mkdir $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/*.h $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/*.c $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/*.ci $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/*.exp $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/README $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/NEWS $(DISTDIR)/src/iodbc
	$(CP_P) $(ODBCDIR)/Changes.log $(DISTDIR)/src/iodbc
	# copy some files from include/ that are not installed:
	mkdir $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/fmappriv.h $(DISTDIR)/include/wx
	# copy wxpresets
	mkdir $(DISTDIR)/build
	mkdir $(DISTDIR)/build/bakefiles
	mkdir $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/presets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/presets/*.bkl $(DISTDIR)/build/bakefiles/wxpresets/presets
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/sample/minimal* $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/sample/config* $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/*.txt $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/aclocal
	$(CP_P) $(WXDIR)/build/aclocal/*.m4 $(DISTDIR)/build/aclocal

# this target is the common part of distribution script for all GUI toolkits,
# but is not used when building wxBase distribution
ALL_GUI_DIST: ALL_DIST
	$(CP_P) $(DOCDIR)/readme.txt $(DISTDIR)/README.txt
	$(CP_P) $(DOCDIR)/$(TOOLKITDIR)/install.txt $(DISTDIR)/INSTALL.txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/changes.txt ; then \
	  $(CP_P) $(DOCDIR)/$(TOOLKITDIR)/changes.txt $(DISTDIR)/CHANGES-$(TOOLKIT).txt ; fi
	$(CP_P) $(DOCDIR)/$(TOOLKITDIR)/readme.txt $(DISTDIR)/README-$(TOOLKIT).txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/todo.txt ; then \
	  $(CP_P) $(DOCDIR)/$(TOOLKITDIR)/todo.txt $(DISTDIR)/TODO.txt ; fi
	mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)
	mkdir $(DISTDIR)/include/wx/generic
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/richtext
	mkdir $(DISTDIR)/include/wx/aui
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/xrc
	-$(CP_P) $(INCDIR)/wx/*.h $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/*.cpp $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/generic/*.h $(DISTDIR)/include/wx/generic
	$(CP_P) $(INCDIR)/wx/html/*.h $(DISTDIR)/include/wx/html
	$(CP_P) $(INCDIR)/wx/richtext/*.h $(DISTDIR)/include/wx/richtext
	$(CP_P) $(INCDIR)/wx/aui/*.h $(DISTDIR)/include/wx/aui
	$(CP_P) $(INCDIR)/wx/unix/*.h $(DISTDIR)/include/wx/unix
	$(CP_P) $(INCDIR)/wx/xml/*.h $(DISTDIR)/include/wx/xml
	$(CP_P) $(INCDIR)/wx/xrc/*.h $(DISTDIR)/include/wx/xrc
	$(CP_P) $(INCDIR)/wx/protocol/*.h $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/art
	mkdir $(DISTDIR)/art/gtk
	mkdir $(DISTDIR)/art/motif
	$(CP_P) $(WXDIR)/art/*.xpm $(DISTDIR)/art
	$(CP_P) $(WXDIR)/art/gtk/*.xpm $(DISTDIR)/art/gtk
	$(CP_P) $(WXDIR)/art/motif/*.xpm $(DISTDIR)/art/motif
	mkdir $(DISTDIR)/src/generic
	mkdir $(DISTDIR)/src/html
	mkdir $(DISTDIR)/src/richtext
	mkdir $(DISTDIR)/src/aui
	mkdir $(DISTDIR)/src/$(TOOLKITDIR)
	mkdir $(DISTDIR)/src/png
	mkdir $(DISTDIR)/src/jpeg
	mkdir $(DISTDIR)/src/tiff
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/xrc
	$(CP_P) $(SRCDIR)/xrc/*.cpp $(DISTDIR)/src/xrc
	-$(CP_P) $(COMMDIR)/*.cpp $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.c $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.inc $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.mms $(DISTDIR)/src/common
	$(CP_P) $(UNIXDIR)/*.cpp $(DISTDIR)/src/unix
	$(CP_P) $(UNIXDIR)/*.mms $(DISTDIR)/src/unix
	$(CP_P) $(GENDIR)/*.cpp $(DISTDIR)/src/generic
	$(CP_P) $(GENDIR)/*.mms $(DISTDIR)/src/generic
	$(CP_P) $(HTMLDIR)/*.cpp $(DISTDIR)/src/html
	$(CP_P) $(RICHTEXTDIR)/*.cpp $(DISTDIR)/src/richtext
	$(CP_P) $(AUIDIR)/*.cpp $(DISTDIR)/src/aui
	$(CP_P) $(PNGDIR)/*.h $(DISTDIR)/src/png
	$(CP_P) $(PNGDIR)/*.c $(DISTDIR)/src/png
	$(CP_P) $(PNGDIR)/README $(DISTDIR)/src/png
	$(CP_P) $(JPEGDIR)/*.h $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/*.c $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/README $(DISTDIR)/src/jpeg
	$(CP_P) $(TIFFDIR)/*.h $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/*.c $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/README $(DISTDIR)/src/tiff

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
	mkdir $(DISTDIR)/include/wx/aui
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/carbon
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/os2
	mkdir $(DISTDIR)/include/wx/palmos
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/carbon
	mkdir $(DISTDIR)/src/mac/carbon/morefilex
	mkdir $(DISTDIR)/src/msdos
	mkdir $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/src/os2
	mkdir $(DISTDIR)/src/palmos
	$(CP_P) $(DOCDIR)/base/readme.txt $(DISTDIR)/README.txt
	$(CP_P) $(WXDIR)/src/common/*.inc $(DISTDIR)/src/common
	$(CP_P) $(WXDIR)/src/common/base.rc $(DISTDIR)/src/common
	list='$(ALL_PORTS_BASE_HEADERS)'; for p in $$list; do \
	  $(CP_P) $(WXDIR)/include/$$p $(DISTDIR)/include/$$p; \
	done
	list='$(ALL_BASE_SOURCES)'; for p in $$list; do \
	  $(CP_P) $(WXDIR)/$$p $(DISTDIR)/$$p; \
	done

	mkdir $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/Makefile.in $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/console.cpp $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/console.dsp $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

	mkdir $(DISTDIR)/utils
	mkdir $(DISTDIR)/utils/HelpGen
	mkdir $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/Makefile.in $(DISTDIR)/utils/HelpGen
	$(CP_P) $(UTILSDIR)/HelpGen/src/Makefile.in $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/src/*.h $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/src/*.cpp $(DISTDIR)/utils/HelpGen/src

	mkdir $(DISTDIR)/utils/tex2rtf
	mkdir $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/*.in $(DISTDIR)/utils/tex2rtf
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.h $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.in $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.cpp $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/tex2rtf.ico $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/tex2rtf.ini $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/tex2rtf.rc $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/tex2rtf.xpm $(DISTDIR)/utils/tex2rtf/src

GTK_DIST: UNIV_DIST
	$(CP_P) $(WXDIR)/wxGTK.spec $(DISTDIR)
	$(CP_P) $(INCDIR)/wx/gtk/*.h $(DISTDIR)/include/wx/gtk
	$(CP_P) $(GTKDIR)/*.h $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.cpp $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.c $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.xbm $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.mms $(DISTDIR)/src/gtk
	mkdir $(DISTDIR)/include/wx/gtk1
	$(CP_P) $(INCDIR)/wx/gtk1/*.h $(DISTDIR)/include/wx/gtk1
	mkdir $(DISTDIR)/src/gtk1
	$(CP_P) $(GTK1DIR)/*.h $(DISTDIR)/src/gtk1
	$(CP_P) $(GTK1DIR)/*.cpp $(DISTDIR)/src/gtk1
	$(CP_P) $(GTK1DIR)/*.c $(DISTDIR)/src/gtk1
	$(CP_P) $(GTK1DIR)/*.xbm $(DISTDIR)/src/gtk1
	$(CP_P) $(GTK1DIR)/*.mms $(DISTDIR)/src/gtk1

	mkdir $(DISTDIR)/include/wx/gtk/gnome
	mkdir $(DISTDIR)/src/gtk/gnome
	$(CP_P) $(INCDIR)/wx/gtk/gnome/*.h $(DISTDIR)/include/wx/gtk/gnome
	$(CP_P) $(GTKDIR)/gnome/*.cpp $(DISTDIR)/src/gtk/gnome

	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(CP_P) $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(CP_P) $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation

	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

X11_DIST: UNIV_DIST
	$(CP_P) $(WXDIR)/wxX11.spec $(DISTDIR)
	$(CP_P) $(INCDIR)/wx/x11/*.h $(DISTDIR)/include/wx/x11
	$(CP_P) $(X11DIR)/*.cpp $(DISTDIR)/src/x11
	$(CP_P) $(X11DIR)/*.c $(DISTDIR)/src/x11
	$(CP_P) $(X11DIR)/*.xbm $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/src/mac
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(CP_P) $(WXDIR)/src/mac/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/include/wx/mac
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(CP_P) $(WXDIR)/include/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

MOTIF_DIST: ALL_GUI_DIST
	$(CP_P) $(WXDIR)/wxMotif.spec $(DISTDIR)
	$(CP_P) $(INCDIR)/wx/motif/*.h $(DISTDIR)/include/wx/motif
	$(CP_P) $(MOTIFDIR)/*.cpp $(DISTDIR)/src/motif
	$(CP_P) $(MOTIFDIR)/*.xbm $(DISTDIR)/src/motif
	mkdir $(DISTDIR)/src/motif/xmcombo
	$(CP_P) $(MOTIFDIR)/xmcombo/*.c $(DISTDIR)/src/motif/xmcombo
	$(CP_P) $(MOTIFDIR)/xmcombo/*.h $(DISTDIR)/src/motif/xmcombo
	$(CP_P) $(MOTIFDIR)/xmcombo/copying.txt $(DISTDIR)/src/motif/xmcombo
	mkdir $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/include/wx/x11
	$(CP_P) $(X11DIR)/pen.cpp $(X11DIR)/brush.cpp $(X11DIR)/utilsx.cpp \
		$(X11DIR)/bitmap.cpp $(X11DIR)/glcanvas.cpp $(X11DIR)/region.cpp \
		$(DISTDIR)/src/x11
	$(CP_P) $(X11INC)/pen.h $(X11INC)/brush.h $(X11INC)/privx.h \
		$(X11INC)/bitmap.h $(X11INC)/glcanvas.h $(X11INC)/private.h $(X11INC)/region.h \
		$(DISTDIR)/include/wx/x11
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

MACX_DIST: ALL_GUI_DIST
	$(CP_P) $(INCDIR)/*.* $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx/mac/carbon
	mkdir $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/carbon/private
	$(CP_P) $(INCDIR)/wx/mac/*.h $(DISTDIR)/include/wx/mac
	$(CP_P) $(INCDIR)/wx/mac/carbon/*.h $(DISTDIR)/include/wx/mac/carbon
	$(CP_P) $(INCDIR)/wx/mac/carbon/private/*.h $(DISTDIR)/include/wx/mac/carbon/private
	$(CP_P) $(INCDIR)/wx/mac/private/*.h $(DISTDIR)/include/wx/mac/private
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(CP_P) $(INCDIR)/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(CP_P) $(MACDIR)/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/src/html/htmlctrl
	mkdir $(DISTDIR)/src/html/htmlctrl/webkit
	$(CP_P) $(WXDIR)/src/html/htmlctrl/webkit/*.mm $(DISTDIR)/src/html/htmlctrl/webkit
	mkdir $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/*.cpp $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/*.mm $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/*.icns $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/*.h $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/*.r $(DISTDIR)/src/mac/carbon
	mkdir $(DISTDIR)/src/mac/carbon/morefile
	$(CP_P) $(MACDIR)/carbon/morefile/*.h $(DISTDIR)/src/mac/carbon/morefile
	$(CP_P) $(MACDIR)/carbon/morefile/*.c $(DISTDIR)/src/mac/carbon/morefile
	mkdir $(DISTDIR)/src/mac/carbon/morefilex
	$(CP_P) $(MACDIR)/carbon/morefilex/*.h $(DISTDIR)/src/mac/carbon/morefilex
	$(CP_P) $(MACDIR)/carbon/morefilex/*.c $(DISTDIR)/src/mac/carbon/morefilex
	$(CP_P) $(MACDIR)/carbon/morefilex/*.cpp $(DISTDIR)/src/mac/carbon/morefilex
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

COCOA_DIST: ALL_GUI_DIST
	$(CP_P) $(INCDIR)/wx/cocoa/*.h $(DISTDIR)/include/wx/cocoa
	$(CP_P) $(COCOADIR)/*.mm $(DISTDIR)/src/cocoa
	$(CP_P) $(COCOADIR)/*.cpp $(DISTDIR)/src/cocoa
	$(CP_P) $(COCOADIR)/*.r $(DISTDIR)/src/cocoa
	mkdir $(DISTDIR)/include/wx/mac/corefoundation
	$(CP_P) $(INCDIR)/wx/mac/corefoundation/*.h $(DISTDIR)/include/wx/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/corefoundation
	$(CP_P) $(MACDIR)/corefoundation/*.cpp $(DISTDIR)/src/mac/corefoundation
	mkdir $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/mac/carbon
	$(CP_P) $(MACDIR)/carbon/wxmac.icns $(DISTDIR)/src/mac/carbon
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

MSW_DIST: UNIV_DIST
	$(CP_P) $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	$(CP_P) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.cur $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.ico $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.bmp $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.manifest $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	$(CP_P) $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	$(CP_P) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.rc $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

MSW_ZIP_TEXT_DIST: ALL_GUI_DIST
	$(CP_P) $(WXDIR)/wxWINE.spec $(DISTDIR)
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/include/wx/msw/wince
	$(CP_P) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.manifest $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	$(CP_P) $(INCDIR)/wx/msw/wince/*.h $(DISTDIR)/include/wx/msw/wince
	mkdir $(DISTDIR)/src/msw
	mkdir $(DISTDIR)/src/msw/ole
	mkdir $(DISTDIR)/src/msw/wince
	$(CP_P) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	$(CP_P) $(MSWDIR)/wince/*.* $(DISTDIR)/src/msw/wince
	$(CP_P) $(SRCDIR)/*.??? $(DISTDIR)/src
	$(CP_P) $(SRCDIR)/*.?? $(DISTDIR)/src
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

UNIV_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/include/wx/univ
	mkdir $(DISTDIR)/src/univ
	mkdir $(DISTDIR)/src/univ/themes
	$(CP_P) $(INCDIR)/wx/univ/*.h $(DISTDIR)/include/wx/univ
	$(CP_P) $(INCDIR)/wx/univ/setup0.h $(DISTDIR)/include/wx/univ/setup.h
	$(CP_P) $(SRCDIR)/univ/*.cpp $(DISTDIR)/src/univ
	$(CP_P) $(SRCDIR)/univ/themes/*.cpp $(DISTDIR)/src/univ/themes

MGL_DIST: UNIV_DIST
	$(CP_P) $(WXDIR)/wxMGL.spec $(DISTDIR)
	$(CP_P) $(INCDIR)/wx/mgl/*.h $(DISTDIR)/include/wx/mgl
	mkdir $(DISTDIR)/include/wx/msdos
	$(CP_P) $(INCDIR)/wx/msdos/*.h $(DISTDIR)/include/wx/msdos
	$(CP_P) $(SRCDIR)/mgl/make* $(DISTDIR)/src/mgl
	$(CP_P) $(SRCDIR)/mgl/*.cpp $(DISTDIR)/src/mgl
	mkdir $(DISTDIR)/src/msdos
	$(CP_P) $(SRCDIR)/msdos/*.cpp $(DISTDIR)/src/msdos
	$(CP_PR) $(WXDIR)/contrib $(DISTDIR)/contrib

DEMOS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/demos
	$(CP_P) $(DEMODIR)/Makefile.in $(DISTDIR)/demos

	mkdir $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/Makefile.in $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/makefile.unx $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/*.cpp $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/*.h $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/*.xpm $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/*.ico $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/*.rc $(DISTDIR)/demos/bombs
	$(CP_P) $(DEMODIR)/bombs/readme.txt $(DISTDIR)/demos/bombs

	$(CP_PR) $(DEMODIR)/dbbrowse $(DISTDIR)/demos/dbbrowse

	mkdir $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/Makefile.in $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/makefile.unx $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.h $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.cpp $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.xpm $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.htm $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.ico $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/*.rc $(DISTDIR)/demos/forty
	$(CP_P) $(DEMODIR)/forty/readme.txt $(DISTDIR)/demos/forty

	mkdir $(DISTDIR)/demos/life
	mkdir $(DISTDIR)/demos/life/bitmaps
	$(CP_P) $(DEMODIR)/life/Makefile.in $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/makefile.unx $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.cpp $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.h $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.xpm $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.inc $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.lif $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.rc $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/*.ico $(DISTDIR)/demos/life
	$(CP_P) $(DEMODIR)/life/bitmaps/*.xpm $(DISTDIR)/demos/life/bitmaps
	$(CP_P) $(DEMODIR)/life/bitmaps/*.bmp $(DISTDIR)/demos/life/bitmaps

	mkdir $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/Makefile.in $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/makefile.unx $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.h $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.cpp $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.xpm $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.dat $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.txt $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.rc $(DISTDIR)/demos/poem
	$(CP_P) $(DEMODIR)/poem/*.ico $(DISTDIR)/demos/poem

	mkdir $(DISTDIR)/demos/fractal
	$(CP_P) $(DEMODIR)/fractal/Makefile.in $(DISTDIR)/demos/fractal
	$(CP_P) $(DEMODIR)/fractal/makefile.unx $(DISTDIR)/demos/fractal
	$(CP_P) $(DEMODIR)/fractal/*.cpp $(DISTDIR)/demos/fractal
	$(CP_P) $(DEMODIR)/fractal/*.rc $(DISTDIR)/demos/fractal
	$(CP_P) $(DEMODIR)/fractal/*.ico $(DISTDIR)/demos/fractal

SAMPLES_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/Makefile.in $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/sample.* $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/artprov
	$(CP_P) $(SAMPDIR)/artprov/Makefile.in $(DISTDIR)/samples/artprov
	$(CP_P) $(SAMPDIR)/artprov/makefile.unx $(DISTDIR)/samples/artprov
	$(CP_P) $(SAMPDIR)/artprov/*.cpp $(DISTDIR)/samples/artprov
	$(CP_P) $(SAMPDIR)/artprov/*.h $(DISTDIR)/samples/artprov
	$(CP_P) $(SAMPDIR)/artprov/*.xpm $(DISTDIR)/samples/artprov

	mkdir $(DISTDIR)/samples/calendar
	$(CP_P) $(SAMPDIR)/calendar/Makefile.in $(DISTDIR)/samples/calendar
	$(CP_P) $(SAMPDIR)/calendar/makefile.unx $(DISTDIR)/samples/calendar
	$(CP_P) $(SAMPDIR)/calendar/*.cpp $(DISTDIR)/samples/calendar

	mkdir $(DISTDIR)/samples/caret
	$(CP_P) $(SAMPDIR)/caret/Makefile.in $(DISTDIR)/samples/caret
	$(CP_P) $(SAMPDIR)/caret/makefile.unx $(DISTDIR)/samples/caret
	$(CP_P) $(SAMPDIR)/caret/*.cpp $(DISTDIR)/samples/caret
	$(CP_P) $(SAMPDIR)/caret/*.xpm $(DISTDIR)/samples/caret

	mkdir $(DISTDIR)/samples/checklst
	$(CP_P) $(SAMPDIR)/checklst/Makefile.in $(DISTDIR)/samples/checklst
	$(CP_P) $(SAMPDIR)/checklst/makefile.unx $(DISTDIR)/samples/checklst
	$(CP_P) $(SAMPDIR)/checklst/*.cpp $(DISTDIR)/samples/checklst
	$(CP_P) $(SAMPDIR)/checklst/*.xpm $(DISTDIR)/samples/checklst
	$(CP_P) $(SAMPDIR)/checklst/*.mms $(DISTDIR)/samples/checklst

	mkdir $(DISTDIR)/samples/config
	$(CP_P) $(SAMPDIR)/config/Makefile.in $(DISTDIR)/samples/config
	$(CP_P) $(SAMPDIR)/config/makefile.unx $(DISTDIR)/samples/config
	$(CP_P) $(SAMPDIR)/config/*.cpp $(DISTDIR)/samples/config

	mkdir $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/*.cpp $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/testdata.fc $(DISTDIR)/samples/console

	mkdir $(DISTDIR)/samples/controls
	mkdir $(DISTDIR)/samples/controls/icons
	$(CP_P) $(SAMPDIR)/controls/Makefile.in $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/makefile.unx $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.cpp $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.xpm $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.bmp $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.ico $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.rc $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/*.mms $(DISTDIR)/samples/controls
	$(CP_P) $(SAMPDIR)/controls/icons/*.??? $(DISTDIR)/samples/controls/icons

	mkdir $(DISTDIR)/samples/db
	$(CP_P) $(SAMPDIR)/db/Makefile.in $(DISTDIR)/samples/db
	$(CP_P) $(SAMPDIR)/db/makefile.unx $(DISTDIR)/samples/db
	$(CP_P) $(SAMPDIR)/db/*.cpp $(DISTDIR)/samples/db
	$(CP_P) $(SAMPDIR)/db/*.h $(DISTDIR)/samples/db
	$(CP_P) $(SAMPDIR)/db/*.xpm $(DISTDIR)/samples/db

	mkdir $(DISTDIR)/samples/debugrpt
	$(CP_P) $(SAMPDIR)/debugrpt/Makefile.in $(DISTDIR)/samples/debugrpt
	$(CP_P) $(SAMPDIR)/debugrpt/*.cpp $(DISTDIR)/samples/debugrpt

	mkdir $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/Makefile.in $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/makefile.unx $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/*.cpp $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/*.h $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/*.txt $(DISTDIR)/samples/dialogs
	$(CP_P) $(SAMPDIR)/dialogs/*.mms $(DISTDIR)/samples/dialogs

	mkdir $(DISTDIR)/samples/dialup
	$(CP_P) $(SAMPDIR)/dialup/Makefile.in $(DISTDIR)/samples/dialup
	$(CP_P) $(SAMPDIR)/dialup/makefile.unx $(DISTDIR)/samples/dialup
	$(CP_P) $(SAMPDIR)/dialup/*.cpp $(DISTDIR)/samples/dialup

	mkdir $(DISTDIR)/samples/display
	$(CP_P) $(SAMPDIR)/display/Makefile.in $(DISTDIR)/samples/display
	$(CP_P) $(SAMPDIR)/display/*.cpp $(DISTDIR)/samples/display

	mkdir $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/Makefile.in $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/makefile.unx $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/*.cpp $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/*.xpm $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/*.txt $(DISTDIR)/samples/dnd
	$(CP_P) $(SAMPDIR)/dnd/*.png $(DISTDIR)/samples/dnd

	mkdir $(DISTDIR)/samples/docview
	$(CP_P) $(SAMPDIR)/docview/Makefile.in $(DISTDIR)/samples/docview
	$(CP_P) $(SAMPDIR)/docview/makefile.unx $(DISTDIR)/samples/docview
	$(CP_P) $(SAMPDIR)/docview/*.cpp $(DISTDIR)/samples/docview
	$(CP_P) $(SAMPDIR)/docview/*.h $(DISTDIR)/samples/docview
	$(CP_P) $(SAMPDIR)/docview/*.xpm $(DISTDIR)/samples/docview

	mkdir $(DISTDIR)/samples/docvwmdi
	$(CP_P) $(SAMPDIR)/docvwmdi/Makefile.in $(DISTDIR)/samples/docvwmdi
	$(CP_P) $(SAMPDIR)/docvwmdi/makefile.unx $(DISTDIR)/samples/docvwmdi
	$(CP_P) $(SAMPDIR)/docvwmdi/*.cpp $(DISTDIR)/samples/docvwmdi
	$(CP_P) $(SAMPDIR)/docvwmdi/*.h $(DISTDIR)/samples/docvwmdi

	mkdir $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/Makefile.in $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/makefile.unx $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/*.cpp $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/*.h $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/*.xpm $(DISTDIR)/samples/dragimag
	$(CP_P) $(SAMPDIR)/dragimag/*.png $(DISTDIR)/samples/dragimag

	mkdir $(DISTDIR)/samples/drawing
	$(CP_P) $(SAMPDIR)/drawing/Makefile.in $(DISTDIR)/samples/drawing
	$(CP_P) $(SAMPDIR)/drawing/makefile.unx $(DISTDIR)/samples/drawing
	$(CP_P) $(SAMPDIR)/drawing/*.cpp $(DISTDIR)/samples/drawing
	$(CP_P) $(SAMPDIR)/drawing/*.xpm $(DISTDIR)/samples/drawing
	$(CP_P) $(SAMPDIR)/drawing/*.bmp $(DISTDIR)/samples/drawing

	mkdir $(DISTDIR)/samples/dynamic
	$(CP_P) $(SAMPDIR)/dynamic/Makefile.in $(DISTDIR)/samples/dynamic
	$(CP_P) $(SAMPDIR)/dynamic/makefile.unx $(DISTDIR)/samples/dynamic
	$(CP_P) $(SAMPDIR)/dynamic/*.cpp $(DISTDIR)/samples/dynamic
	$(CP_P) $(SAMPDIR)/dynamic/*.xpm $(DISTDIR)/samples/dynamic

	mkdir $(DISTDIR)/samples/erase
	$(CP_P) $(SAMPDIR)/erase/Makefile.in $(DISTDIR)/samples/erase
	$(CP_P) $(SAMPDIR)/erase/makefile.unx $(DISTDIR)/samples/erase
	$(CP_P) $(SAMPDIR)/erase/*.cpp $(DISTDIR)/samples/erase
	$(CP_P) $(SAMPDIR)/erase/*.xml $(DISTDIR)/samples/erase
	$(CP_P) $(SAMPDIR)/erase/*.xpm $(DISTDIR)/samples/erase

	mkdir $(DISTDIR)/samples/event
	$(CP_P) $(SAMPDIR)/event/Makefile.in $(DISTDIR)/samples/event
	$(CP_P) $(SAMPDIR)/event/makefile.unx $(DISTDIR)/samples/event
	$(CP_P) $(SAMPDIR)/event/*.cpp $(DISTDIR)/samples/event

	mkdir $(DISTDIR)/samples/except
	$(CP_P) $(SAMPDIR)/except/Makefile.in $(DISTDIR)/samples/except
	$(CP_P) $(SAMPDIR)/except/*.cpp $(DISTDIR)/samples/except

	mkdir $(DISTDIR)/samples/exec
	$(CP_P) $(SAMPDIR)/exec/Makefile.in $(DISTDIR)/samples/exec
	$(CP_P) $(SAMPDIR)/exec/makefile.unx $(DISTDIR)/samples/exec
	$(CP_P) $(SAMPDIR)/exec/*.cpp $(DISTDIR)/samples/exec

	mkdir $(DISTDIR)/samples/font
	$(CP_P) $(SAMPDIR)/font/Makefile.in $(DISTDIR)/samples/font
	$(CP_P) $(SAMPDIR)/font/makefile.unx $(DISTDIR)/samples/font
	$(CP_P) $(SAMPDIR)/font/*.cpp $(DISTDIR)/samples/font

	mkdir $(DISTDIR)/samples/grid
	$(CP_P) $(SAMPDIR)/grid/Makefile.in $(DISTDIR)/samples/grid
	$(CP_P) $(SAMPDIR)/grid/makefile.unx $(DISTDIR)/samples/grid
	$(CP_P) $(SAMPDIR)/grid/*.cpp $(SAMPDIR)/grid/*.h $(DISTDIR)/samples/grid

	mkdir $(DISTDIR)/samples/help
	mkdir $(DISTDIR)/samples/help/doc
	$(CP_P) $(SAMPDIR)/help/Makefile.in $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/makefile.unx $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.cpp $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.h* $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.xpm $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.bmp $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.gif $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/cshelp.txt $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.chm $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.cnt $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/doc.zip $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/doc/*.* $(DISTDIR)/samples/help/doc

	mkdir $(DISTDIR)/samples/htlbox
	$(CP_P) $(SAMPDIR)/htlbox/Makefile.in $(DISTDIR)/samples/htlbox
	$(CP_P) $(SAMPDIR)/htlbox/makefile.unx $(DISTDIR)/samples/htlbox
	$(CP_P) $(SAMPDIR)/htlbox/*.cpp $(DISTDIR)/samples/htlbox
	$(CP_P) $(SAMPDIR)/htlbox/*.xpm $(DISTDIR)/samples/htlbox

	mkdir $(DISTDIR)/samples/html
	$(CP_P) $(SAMPDIR)/html/Makefile.in $(DISTDIR)/samples/html
	mkdir $(DISTDIR)/samples/html/about
	$(CP_P) $(SAMPDIR)/html/about/Makefile.in $(DISTDIR)/samples/html/about
	$(CP_P) $(SAMPDIR)/html/about/*.cpp $(DISTDIR)/samples/html/about
	mkdir $(DISTDIR)/samples/html/about/data
	$(CP_P) $(SAMPDIR)/html/about/data/*.htm $(DISTDIR)/samples/html/about/data
	$(CP_P) $(SAMPDIR)/html/about/data/*.png $(DISTDIR)/samples/html/about/data
	mkdir $(DISTDIR)/samples/html/help
	$(CP_P) $(SAMPDIR)/html/help/Makefile.in $(DISTDIR)/samples/html/help
	$(CP_P) $(SAMPDIR)/html/help/*.cpp $(DISTDIR)/samples/html/help
	mkdir $(DISTDIR)/samples/html/help/helpfiles
	$(CP_P) $(SAMPDIR)/html/help/helpfiles/*.??? $(DISTDIR)/samples/html/help/helpfiles
	mkdir $(DISTDIR)/samples/html/helpview
	$(CP_P) $(SAMPDIR)/html/helpview/Makefile.in $(DISTDIR)/samples/html/helpview
	$(CP_P) $(SAMPDIR)/html/helpview/*.cpp $(DISTDIR)/samples/html/helpview
	$(CP_P) $(SAMPDIR)/html/helpview/*.zip $(DISTDIR)/samples/html/helpview
	mkdir $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/Makefile.in $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/*.cpp $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/*.htm $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/*.gif $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/*.ico $(DISTDIR)/samples/html/printing
	mkdir $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/Makefile.in $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.cpp $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.bmp $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.png $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.gif $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.htm $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.html $(DISTDIR)/samples/html/test
	mkdir $(DISTDIR)/samples/html/virtual
	$(CP_P) $(SAMPDIR)/html/virtual/Makefile.in $(DISTDIR)/samples/html/virtual
	$(CP_P) $(SAMPDIR)/html/virtual/*.cpp $(DISTDIR)/samples/html/virtual
	$(CP_P) $(SAMPDIR)/html/virtual/*.htm $(DISTDIR)/samples/html/virtual
	mkdir $(DISTDIR)/samples/html/widget
	$(CP_P) $(SAMPDIR)/html/widget/Makefile.in $(DISTDIR)/samples/html/widget
	$(CP_P) $(SAMPDIR)/html/widget/*.cpp $(DISTDIR)/samples/html/widget
	$(CP_P) $(SAMPDIR)/html/widget/*.htm $(DISTDIR)/samples/html/widget
	mkdir $(DISTDIR)/samples/html/zip
	$(CP_P) $(SAMPDIR)/html/zip/Makefile.in $(DISTDIR)/samples/html/zip
	$(CP_P) $(SAMPDIR)/html/zip/*.cpp $(DISTDIR)/samples/html/zip
	$(CP_P) $(SAMPDIR)/html/zip/*.htm $(DISTDIR)/samples/html/zip
	$(CP_P) $(SAMPDIR)/html/zip/*.zip $(DISTDIR)/samples/html/zip

	mkdir $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/Makefile.in $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/makefile.unx $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/*.cpp $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/*.mms $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/horse*.* $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/smile.xbm $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/smile.xpm $(DISTDIR)/samples/image

	mkdir $(DISTDIR)/samples/internat
	$(CP_P) $(SAMPDIR)/internat/Makefile.in $(DISTDIR)/samples/internat
	$(CP_P) $(SAMPDIR)/internat/makefile.unx $(DISTDIR)/samples/internat
	$(CP_P) $(SAMPDIR)/internat/*.cpp $(DISTDIR)/samples/internat
	$(CP_P) $(SAMPDIR)/internat/*.xpm $(DISTDIR)/samples/internat
	$(CP_P) $(SAMPDIR)/internat/*.txt $(DISTDIR)/samples/internat
	for f in `(cd $(SAMPDIR); find internat -name '*.[mp]o' -print)`; do \
	    mkdir -p $(DISTDIR)/samples/`dirname $$f`;  \
	    $(CP_P) $(SAMPDIR)/$$f $(DISTDIR)/samples/$$f; \
	done

	mkdir $(DISTDIR)/samples/ipc
	$(CP_P) $(SAMPDIR)/ipc/Makefile.in $(DISTDIR)/samples/ipc
	$(CP_P) $(SAMPDIR)/ipc/makefile.unx $(DISTDIR)/samples/ipc
	$(CP_P) $(SAMPDIR)/ipc/*.cpp $(DISTDIR)/samples/ipc
	$(CP_P) $(SAMPDIR)/ipc/*.h $(DISTDIR)/samples/ipc
	$(CP_P) $(SAMPDIR)/ipc/*.xpm $(DISTDIR)/samples/ipc

	mkdir $(DISTDIR)/samples/joytest
	$(CP_P) $(SAMPDIR)/joytest/Makefile.in $(DISTDIR)/samples/joytest
	$(CP_P) $(SAMPDIR)/joytest/makefile.unx $(DISTDIR)/samples/joytest
	$(CP_P) $(SAMPDIR)/joytest/*.cpp $(DISTDIR)/samples/joytest
	$(CP_P) $(SAMPDIR)/joytest/*.h $(DISTDIR)/samples/joytest
	$(CP_P) $(SAMPDIR)/joytest/*.wav $(DISTDIR)/samples/joytest

	mkdir $(DISTDIR)/samples/keyboard
	$(CP_P) $(SAMPDIR)/keyboard/Makefile.in $(DISTDIR)/samples/keyboard
	$(CP_P) $(SAMPDIR)/keyboard/*.cpp $(DISTDIR)/samples/keyboard

	mkdir $(DISTDIR)/samples/layout
	$(CP_P) $(SAMPDIR)/layout/Makefile.in $(DISTDIR)/samples/layout
	$(CP_P) $(SAMPDIR)/layout/makefile.unx $(DISTDIR)/samples/layout
	$(CP_P) $(SAMPDIR)/layout/*.cpp $(DISTDIR)/samples/layout
	$(CP_P) $(SAMPDIR)/layout/*.h $(DISTDIR)/samples/layout

	mkdir $(DISTDIR)/samples/listbox
	$(CP_P) $(SAMPDIR)/listbox/Makefile.in $(DISTDIR)/samples/listbox
	$(CP_P) $(SAMPDIR)/listbox/*.cpp $(DISTDIR)/samples/listbox

	mkdir $(DISTDIR)/samples/listctrl
	mkdir $(DISTDIR)/samples/listctrl/bitmaps
	$(CP_P) $(SAMPDIR)/listctrl/Makefile.in $(DISTDIR)/samples/listctrl
	$(CP_P) $(SAMPDIR)/listctrl/makefile.unx $(DISTDIR)/samples/listctrl
	$(CP_P) $(SAMPDIR)/listctrl/*.cpp $(DISTDIR)/samples/listctrl
	$(CP_P) $(SAMPDIR)/listctrl/*.h $(DISTDIR)/samples/listctrl
	$(CP_P) $(SAMPDIR)/listctrl/*.xpm $(DISTDIR)/samples/listctrl
	$(CP_P) $(SAMPDIR)/listctrl/bitmaps/*.xpm $(DISTDIR)/samples/listctrl/bitmaps

	mkdir $(DISTDIR)/samples/mediaplayer
	$(CP_P) $(SAMPDIR)/mediaplayer/Makefile.in $(DISTDIR)/samples/mediaplayer
	$(CP_P) $(SAMPDIR)/mediaplayer/*.cpp $(DISTDIR)/samples/mediaplayer

	mkdir $(DISTDIR)/samples/mdi
	mkdir $(DISTDIR)/samples/mdi/bitmaps
	$(CP_P) $(SAMPDIR)/mdi/Makefile.in $(DISTDIR)/samples/mdi
	$(CP_P) $(SAMPDIR)/mdi/makefile.unx $(DISTDIR)/samples/mdi
	$(CP_P) $(SAMPDIR)/mdi/*.cpp $(DISTDIR)/samples/mdi
	$(CP_P) $(SAMPDIR)/mdi/*.h $(DISTDIR)/samples/mdi
	$(CP_P) $(SAMPDIR)/mdi/*.xpm $(DISTDIR)/samples/mdi
	$(CP_P) $(SAMPDIR)/mdi/bitmaps/*.xpm $(DISTDIR)/samples/mdi/bitmaps

	mkdir $(DISTDIR)/samples/memcheck
	$(CP_P) $(SAMPDIR)/memcheck/Makefile.in $(DISTDIR)/samples/memcheck
	$(CP_P) $(SAMPDIR)/memcheck/makefile.unx $(DISTDIR)/samples/memcheck
	$(CP_P) $(SAMPDIR)/memcheck/*.cpp $(DISTDIR)/samples/memcheck
	$(CP_P) $(SAMPDIR)/memcheck/*.xpm $(DISTDIR)/samples/memcheck

	mkdir $(DISTDIR)/samples/menu
	$(CP_P) $(SAMPDIR)/menu/Makefile.in $(DISTDIR)/samples/menu
	$(CP_P) $(SAMPDIR)/menu/makefile.unx $(DISTDIR)/samples/menu
	$(CP_P) $(SAMPDIR)/menu/*.cpp $(DISTDIR)/samples/menu
	$(CP_P) $(SAMPDIR)/menu/*.xpm $(DISTDIR)/samples/menu

	mkdir $(DISTDIR)/samples/minifram
	mkdir $(DISTDIR)/samples/minifram/bitmaps
	$(CP_P) $(SAMPDIR)/minifram/Makefile.in $(DISTDIR)/samples/minifram
	$(CP_P) $(SAMPDIR)/minifram/makefile.unx $(DISTDIR)/samples/minifram
	$(CP_P) $(SAMPDIR)/minifram/*.cpp $(DISTDIR)/samples/minifram
	$(CP_P) $(SAMPDIR)/minifram/*.h $(DISTDIR)/samples/minifram
	$(CP_P) $(SAMPDIR)/minifram/*.xpm $(DISTDIR)/samples/minifram
	$(CP_P) $(SAMPDIR)/minifram/bitmaps/*.xpm $(DISTDIR)/samples/minifram/bitmaps

	mkdir $(DISTDIR)/samples/minimal
	$(CP_P) $(SAMPDIR)/minimal/Makefile.in $(DISTDIR)/samples/minimal
	$(CP_P) $(SAMPDIR)/minimal/makefile.unx $(DISTDIR)/samples/minimal
	$(CP_P) $(SAMPDIR)/minimal/*.cpp $(DISTDIR)/samples/minimal
	$(CP_P) $(SAMPDIR)/minimal/*.mms $(DISTDIR)/samples/minimal

	mkdir $(DISTDIR)/samples/mobile
	$(CP_P) $(SAMPDIR)/mobile/Makefile.in $(DISTDIR)/samples/mobile
	mkdir $(DISTDIR)/samples/mobile/wxedit
	$(CP_P) $(SAMPDIR)/mobile/wxedit/Makefile.in $(DISTDIR)/samples/mobile/wxedit
	$(CP_P) $(SAMPDIR)/mobile/wxedit/*.cpp $(DISTDIR)/samples/mobile/wxedit
	$(CP_P) $(SAMPDIR)/mobile/wxedit/*.h $(DISTDIR)/samples/mobile/wxedit
	mkdir $(DISTDIR)/samples/mobile/styles
	$(CP_P) $(SAMPDIR)/mobile/styles/Makefile.in $(DISTDIR)/samples/mobile/styles
	$(CP_P) $(SAMPDIR)/mobile/styles/*.cpp $(DISTDIR)/samples/mobile/styles
	$(CP_P) $(SAMPDIR)/mobile/styles/*.h $(DISTDIR)/samples/mobile/styles
	$(CP_P) $(SAMPDIR)/mobile/styles/*.jpg $(DISTDIR)/samples/mobile/styles

	mkdir $(DISTDIR)/samples/multimon
	$(CP_P) $(SAMPDIR)/multimon/Makefile.in $(DISTDIR)/samples/multimon
	$(CP_P) $(SAMPDIR)/multimon/makefile.unx $(DISTDIR)/samples/multimon
	$(CP_P) $(SAMPDIR)/multimon/*.cpp $(DISTDIR)/samples/multimon

	mkdir $(DISTDIR)/samples/notebook
	$(CP_P) $(SAMPDIR)/notebook/Makefile.in $(DISTDIR)/samples/notebook
	$(CP_P) $(SAMPDIR)/notebook/makefile.unx $(DISTDIR)/samples/notebook
	$(CP_P) $(SAMPDIR)/notebook/*.cpp $(DISTDIR)/samples/notebook
	$(CP_P) $(SAMPDIR)/notebook/*.h $(DISTDIR)/samples/notebook

	mkdir $(DISTDIR)/samples/opengl
	$(CP_P) $(SAMPDIR)/opengl/Makefile.in $(DISTDIR)/samples/opengl
	mkdir $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/Makefile.in $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/makefile.unx $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/*.cpp $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/*.c $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/*.h $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/*.dxf.gz $(DISTDIR)/samples/opengl/penguin
	mkdir $(DISTDIR)/samples/opengl/cube
	$(CP_P) $(SAMPDIR)/opengl/cube/Makefile.in $(DISTDIR)/samples/opengl/cube
	$(CP_P) $(SAMPDIR)/opengl/cube/makefile.unx $(DISTDIR)/samples/opengl/cube
	$(CP_P) $(SAMPDIR)/opengl/cube/*.cpp $(DISTDIR)/samples/opengl/cube
	$(CP_P) $(SAMPDIR)/opengl/cube/*.h $(DISTDIR)/samples/opengl/cube
	mkdir $(DISTDIR)/samples/opengl/isosurf
	$(CP_P) $(SAMPDIR)/opengl/isosurf/Makefile.in $(DISTDIR)/samples/opengl/isosurf
	$(CP_P) $(SAMPDIR)/opengl/isosurf/makefile.unx $(DISTDIR)/samples/opengl/isosurf
	$(CP_P) $(SAMPDIR)/opengl/isosurf/*.cpp $(DISTDIR)/samples/opengl/isosurf
	$(CP_P) $(SAMPDIR)/opengl/isosurf/*.h $(DISTDIR)/samples/opengl/isosurf
	$(CP_P) $(SAMPDIR)/opengl/isosurf/*.gz $(DISTDIR)/samples/opengl/isosurf

	mkdir $(DISTDIR)/samples/png
	$(CP_P) $(SAMPDIR)/png/Makefile.in $(DISTDIR)/samples/png
	$(CP_P) $(SAMPDIR)/png/makefile.unx $(DISTDIR)/samples/png
	$(CP_P) $(SAMPDIR)/png/*.cpp $(DISTDIR)/samples/png
	$(CP_P) $(SAMPDIR)/png/*.h $(DISTDIR)/samples/png
	$(CP_P) $(SAMPDIR)/png/*.png $(DISTDIR)/samples/png

	mkdir $(DISTDIR)/samples/popup
	$(CP_P) $(SAMPDIR)/popup/Makefile.in $(DISTDIR)/samples/popup
	$(CP_P) $(SAMPDIR)/popup/makefile.unx $(DISTDIR)/samples/popup
	$(CP_P) $(SAMPDIR)/popup/*.cpp $(DISTDIR)/samples/popup

	mkdir $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/Makefile.in $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/makefile.unx $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/*.cpp $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/*.h $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/*.xpm $(DISTDIR)/samples/printing
	$(CP_P) $(SAMPDIR)/printing/*.rc $(DISTDIR)/samples/printing

	mkdir $(DISTDIR)/samples/render
	$(CP_P) $(SAMPDIR)/render/Makefile.in $(DISTDIR)/samples/render
	$(CP_P) $(SAMPDIR)/render/makefile.unx $(DISTDIR)/samples/render
	$(CP_P) $(SAMPDIR)/render/*.cpp $(DISTDIR)/samples/render

	mkdir $(DISTDIR)/samples/rotate
	$(CP_P) $(SAMPDIR)/rotate/Makefile.in $(DISTDIR)/samples/rotate
	$(CP_P) $(SAMPDIR)/rotate/makefile.unx $(DISTDIR)/samples/rotate
	$(CP_P) $(SAMPDIR)/rotate/*.cpp $(DISTDIR)/samples/rotate
	$(CP_P) $(SAMPDIR)/rotate/*.png $(DISTDIR)/samples/rotate

	mkdir $(DISTDIR)/samples/richtext
	mkdir $(DISTDIR)/samples/richtext/bitmaps
	$(CP_P) $(SAMPDIR)/richtext/Makefile.in $(DISTDIR)/samples/richtext
	$(CP_P) $(SAMPDIR)/richtext/*.cpp $(DISTDIR)/samples/richtext
	$(CP_P) $(SAMPDIR)/richtext/bitmaps/*.xpm $(DISTDIR)/samples/richtext/bitmaps
	$(CP_P) $(SAMPDIR)/richtext/readme.txt $(DISTDIR)/samples/richtext
	$(CP_P) $(SAMPDIR)/richtext/todo.txt $(DISTDIR)/samples/richtext

	mkdir $(DISTDIR)/samples/aui
	$(CP_P) $(SAMPDIR)/aui/Makefile.in $(DISTDIR)/samples/aui
	$(CP_P) $(SAMPDIR)/aui/*.cpp $(DISTDIR)/samples/aui

	mkdir $(DISTDIR)/samples/propsize
	$(CP_P) $(SAMPDIR)/propsize/Makefile.in $(DISTDIR)/samples/propsize
	$(CP_P) $(SAMPDIR)/propsize/makefile.unx $(DISTDIR)/samples/propsize
	$(CP_P) $(SAMPDIR)/propsize/*.cpp $(DISTDIR)/samples/propsize
	$(CP_P) $(SAMPDIR)/propsize/*.xpm $(DISTDIR)/samples/propsize

	mkdir $(DISTDIR)/samples/sashtest
	$(CP_P) $(SAMPDIR)/sashtest/Makefile.in $(DISTDIR)/samples/sashtest
	$(CP_P) $(SAMPDIR)/sashtest/makefile.unx $(DISTDIR)/samples/sashtest
	$(CP_P) $(SAMPDIR)/sashtest/*.cpp $(DISTDIR)/samples/sashtest
	$(CP_P) $(SAMPDIR)/sashtest/*.h $(DISTDIR)/samples/sashtest

	mkdir $(DISTDIR)/samples/scroll
	$(CP_P) $(SAMPDIR)/scroll/Makefile.in $(DISTDIR)/samples/scroll
	$(CP_P) $(SAMPDIR)/scroll/makefile.unx $(DISTDIR)/samples/scroll
	$(CP_P) $(SAMPDIR)/scroll/*.cpp $(DISTDIR)/samples/scroll

	mkdir $(DISTDIR)/samples/scrollsub
	$(CP_P) $(SAMPDIR)/scrollsub/Makefile.in $(DISTDIR)/samples/scrollsub
	$(CP_P) $(SAMPDIR)/scrollsub/makefile.unx $(DISTDIR)/samples/scrollsub
	$(CP_P) $(SAMPDIR)/scrollsub/*.cpp $(DISTDIR)/samples/scrollsub

	mkdir $(DISTDIR)/samples/shaped
	$(CP_P) $(SAMPDIR)/shaped/Makefile.in $(DISTDIR)/samples/shaped
	$(CP_P) $(SAMPDIR)/shaped/makefile.unx $(DISTDIR)/samples/shaped
	$(CP_P) $(SAMPDIR)/shaped/*.cpp $(DISTDIR)/samples/shaped
	$(CP_P) $(SAMPDIR)/shaped/*.png $(DISTDIR)/samples/shaped

	mkdir $(DISTDIR)/samples/sockets
	$(CP_P) $(SAMPDIR)/sockets/Makefile.in $(DISTDIR)/samples/sockets
	$(CP_P) $(SAMPDIR)/sockets/makefile.unx $(DISTDIR)/samples/sockets
	$(CP_P) $(SAMPDIR)/sockets/*.cpp $(DISTDIR)/samples/sockets
	$(CP_P) $(SAMPDIR)/sockets/*.xpm $(DISTDIR)/samples/sockets

	mkdir $(DISTDIR)/samples/sound
	$(CP_P) $(SAMPDIR)/sound/Makefile.in $(DISTDIR)/samples/sound
	$(CP_P) $(SAMPDIR)/sound/*.cpp $(DISTDIR)/samples/sound
	$(CP_P) $(SAMPDIR)/sound/*.wav $(DISTDIR)/samples/sound

	mkdir $(DISTDIR)/samples/splash
	$(CP_P) $(SAMPDIR)/splash/Makefile.in $(DISTDIR)/samples/splash
	$(CP_P) $(SAMPDIR)/splash/*.cpp $(DISTDIR)/samples/splash
	$(CP_P) $(SAMPDIR)/splash/*.png $(DISTDIR)/samples/splash
	$(CP_P) $(SAMPDIR)/splash/*.mpg $(DISTDIR)/samples/splash
	$(CP_P) $(SAMPDIR)/splash/*.xpm $(DISTDIR)/samples/splash

	mkdir $(DISTDIR)/samples/splitter
	$(CP_P) $(SAMPDIR)/splitter/Makefile.in $(DISTDIR)/samples/splitter
	$(CP_P) $(SAMPDIR)/splitter/makefile.unx $(DISTDIR)/samples/splitter
	$(CP_P) $(SAMPDIR)/splitter/*.cpp $(DISTDIR)/samples/splitter

	mkdir $(DISTDIR)/samples/statbar
	$(CP_P) $(SAMPDIR)/statbar/Makefile.in $(DISTDIR)/samples/statbar
	$(CP_P) $(SAMPDIR)/statbar/makefile.unx $(DISTDIR)/samples/statbar
	$(CP_P) $(SAMPDIR)/statbar/*.cpp $(DISTDIR)/samples/statbar
	$(CP_P) $(SAMPDIR)/statbar/*.xpm $(DISTDIR)/samples/statbar

	mkdir $(DISTDIR)/samples/taskbar
	$(CP_P) $(SAMPDIR)/taskbar/Makefile.in $(DISTDIR)/samples/taskbar
	$(CP_P) $(SAMPDIR)/taskbar/*.cpp $(DISTDIR)/samples/taskbar
	$(CP_P) $(SAMPDIR)/taskbar/*.h $(DISTDIR)/samples/taskbar
	$(CP_P) $(SAMPDIR)/taskbar/*.xpm $(DISTDIR)/samples/taskbar

	mkdir $(DISTDIR)/samples/text
	$(CP_P) $(SAMPDIR)/text/Makefile.in $(DISTDIR)/samples/text
	$(CP_P) $(SAMPDIR)/text/makefile.unx $(DISTDIR)/samples/text
	$(CP_P) $(SAMPDIR)/text/*.cpp $(DISTDIR)/samples/text
	$(CP_P) $(SAMPDIR)/text/*.xpm $(DISTDIR)/samples/text

	mkdir $(DISTDIR)/samples/thread
	$(CP_P) $(SAMPDIR)/thread/Makefile.in $(DISTDIR)/samples/thread
	$(CP_P) $(SAMPDIR)/thread/makefile.unx $(DISTDIR)/samples/thread
	$(CP_P) $(SAMPDIR)/thread/*.cpp $(DISTDIR)/samples/thread

	mkdir $(DISTDIR)/samples/toolbar
	$(CP_P) $(SAMPDIR)/toolbar/Makefile.in $(DISTDIR)/samples/toolbar
	$(CP_P) $(SAMPDIR)/toolbar/makefile.unx $(DISTDIR)/samples/toolbar
	$(CP_P) $(SAMPDIR)/toolbar/*.cpp $(DISTDIR)/samples/toolbar
	$(CP_P) $(SAMPDIR)/toolbar/*.xpm $(DISTDIR)/samples/toolbar
	mkdir $(DISTDIR)/samples/toolbar/bitmaps
	$(CP_P) $(SAMPDIR)/toolbar/bitmaps/*.xpm $(DISTDIR)/samples/toolbar/bitmaps

	mkdir $(DISTDIR)/samples/treectrl
	$(CP_P) $(SAMPDIR)/treectrl/Makefile.in $(DISTDIR)/samples/treectrl
	$(CP_P) $(SAMPDIR)/treectrl/makefile.unx $(DISTDIR)/samples/treectrl
	$(CP_P) $(SAMPDIR)/treectrl/*.cpp $(DISTDIR)/samples/treectrl
	$(CP_P) $(SAMPDIR)/treectrl/*.h $(DISTDIR)/samples/treectrl
	$(CP_P) $(SAMPDIR)/treectrl/*.xpm $(DISTDIR)/samples/treectrl

	mkdir $(DISTDIR)/samples/typetest
	$(CP_P) $(SAMPDIR)/typetest/Makefile.in $(DISTDIR)/samples/typetest
	$(CP_P) $(SAMPDIR)/typetest/makefile.unx $(DISTDIR)/samples/typetest
	$(CP_P) $(SAMPDIR)/typetest/*.cpp $(DISTDIR)/samples/typetest
	$(CP_P) $(SAMPDIR)/typetest/*.h $(DISTDIR)/samples/typetest
	$(CP_P) $(SAMPDIR)/typetest/*.xpm $(DISTDIR)/samples/typetest

	mkdir $(DISTDIR)/samples/validate
	$(CP_P) $(SAMPDIR)/validate/Makefile.in $(DISTDIR)/samples/validate
	$(CP_P) $(SAMPDIR)/validate/makefile.unx $(DISTDIR)/samples/validate
	$(CP_P) $(SAMPDIR)/validate/*.cpp $(DISTDIR)/samples/validate
	$(CP_P) $(SAMPDIR)/validate/*.h $(DISTDIR)/samples/validate
	$(CP_P) $(SAMPDIR)/validate/*.xpm $(DISTDIR)/samples/validate

	mkdir $(DISTDIR)/samples/vscroll
	$(CP_P) $(SAMPDIR)/vscroll/Makefile.in $(DISTDIR)/samples/vscroll
	$(CP_P) $(SAMPDIR)/vscroll/*.cpp $(DISTDIR)/samples/vscroll

	mkdir $(DISTDIR)/samples/wizard
	$(CP_P) $(SAMPDIR)/wizard/Makefile.in $(DISTDIR)/samples/wizard
	$(CP_P) $(SAMPDIR)/wizard/makefile.unx $(DISTDIR)/samples/wizard
	$(CP_P) $(SAMPDIR)/wizard/*.cpp $(DISTDIR)/samples/wizard
	$(CP_P) $(SAMPDIR)/wizard/*.xpm $(DISTDIR)/samples/wizard

	mkdir $(DISTDIR)/samples/widgets
	mkdir $(DISTDIR)/samples/widgets/icons
	$(CP_P) $(SAMPDIR)/widgets/Makefile.in $(DISTDIR)/samples/widgets
	$(CP_P) $(SAMPDIR)/widgets/*.cpp $(DISTDIR)/samples/widgets
	$(CP_P) $(SAMPDIR)/widgets/*.h $(DISTDIR)/samples/widgets
	$(CP_P) $(SAMPDIR)/widgets/icons/*.xpm $(DISTDIR)/samples/widgets/icons

	mkdir $(DISTDIR)/samples/xrc
	mkdir $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/Makefile.in $(DISTDIR)/samples/xrc
	$(CP_P) $(SAMPDIR)/xrc/*.cpp $(DISTDIR)/samples/xrc
	$(CP_P) $(SAMPDIR)/xrc/*.h $(DISTDIR)/samples/xrc
	$(CP_P) $(SAMPDIR)/xrc/*.rc $(DISTDIR)/samples/xrc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.xpm $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.xrc $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.gif $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.ico $(DISTDIR)/samples/xrc/rc

UTILS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/utils
	$(CP_P) $(UTILSDIR)/Makefile.in $(DISTDIR)/utils

	mkdir $(DISTDIR)/utils/tex2rtf
	mkdir $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/*.in $(DISTDIR)/utils/tex2rtf
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.h $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.in $(DISTDIR)/utils/tex2rtf/src
	$(CP_P) $(UTILSDIR)/tex2rtf/src/*.cpp $(DISTDIR)/utils/tex2rtf/src
	-$(CP_P) $(UTILSDIR)/tex2rtf/src/tex2rtf.* $(DISTDIR)/utils/tex2rtf/src

	mkdir $(DISTDIR)/utils/configtool
	mkdir $(DISTDIR)/utils/configtool/configs
	mkdir $(DISTDIR)/utils/configtool/docs
	mkdir $(DISTDIR)/utils/configtool/src
	mkdir $(DISTDIR)/utils/configtool/src/bitmaps
	mkdir $(DISTDIR)/utils/configtool/scripts
	$(CP_P) $(UTILSDIR)/configtool/*.in $(DISTDIR)/utils/configtool
	$(CP_P) $(UTILSDIR)/configtool/configs/*.wxs $(DISTDIR)/utils/configtool/configs
	$(CP_P) $(UTILSDIR)/configtool/docs/*.txt $(DISTDIR)/utils/configtool/docs
	$(CP_P) $(UTILSDIR)/configtool/src/*.h $(DISTDIR)/utils/configtool/src
	$(CP_P) $(UTILSDIR)/configtool/src/*.in $(DISTDIR)/utils/configtool/src
	$(CP_P) $(UTILSDIR)/configtool/src/*.cpp $(DISTDIR)/utils/configtool/src
	$(CP_P) $(UTILSDIR)/configtool/src/bitmaps/*.xpm $(DISTDIR)/utils/configtool/src/bitmaps
	$(CP_P) $(UTILSDIR)/configtool/scripts/????* $(DISTDIR)/utils/configtool/scripts

	mkdir $(DISTDIR)/utils/emulator
	mkdir $(DISTDIR)/utils/emulator/src
	mkdir $(DISTDIR)/utils/emulator/docs
	$(CP_P) $(UTILSDIR)/emulator/*.in $(DISTDIR)/utils/emulator
	$(CP_P) $(UTILSDIR)/emulator/src/*.h $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/src/*.in $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/src/*.cpp $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/src/*.jpg $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/src/*.wxe $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/src/*.xpm $(DISTDIR)/utils/emulator/src
	$(CP_P) $(UTILSDIR)/emulator/docs/*.txt $(DISTDIR)/utils/emulator/docs
	$(CP_P) $(UTILSDIR)/emulator/docs/*.jpg $(DISTDIR)/utils/emulator/docs

	mkdir $(DISTDIR)/utils/hhp2cached
	$(CP_P) $(UTILSDIR)/hhp2cached/Makefile.in $(DISTDIR)/utils/hhp2cached
	$(CP_P) $(UTILSDIR)/hhp2cached/*.cpp $(DISTDIR)/utils/hhp2cached
	$(CP_P) $(UTILSDIR)/hhp2cached/*.rc $(DISTDIR)/utils/hhp2cached

	mkdir $(DISTDIR)/utils/HelpGen
	mkdir $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/Makefile.in $(DISTDIR)/utils/HelpGen
	$(CP_P) $(UTILSDIR)/HelpGen/src/Makefile.in $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/src/*.h $(DISTDIR)/utils/HelpGen/src
	$(CP_P) $(UTILSDIR)/HelpGen/src/*.cpp $(DISTDIR)/utils/HelpGen/src

	mkdir $(DISTDIR)/utils/helpview
	mkdir $(DISTDIR)/utils/helpview/src
	mkdir $(DISTDIR)/utils/helpview/src/bitmaps
	$(CP_P) $(UTILSDIR)/helpview/Makefile.in $(DISTDIR)/utils/helpview
	$(CP_P) $(UTILSDIR)/helpview/src/*.h $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/*.cpp $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/*.rc $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/*.ico $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/readme.txt $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/Makefile.in $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/test.zip $(DISTDIR)/utils/helpview/src
	$(CP_P) $(UTILSDIR)/helpview/src/bitmaps/*.xpm $(DISTDIR)/utils/helpview/src/bitmaps

	mkdir $(DISTDIR)/utils/wxrc
	$(CP_P) $(UTILSDIR)/wxrc/Makefile.in $(DISTDIR)/utils/wxrc
	$(CP_P) $(UTILSDIR)/wxrc/*.cpp $(DISTDIR)/utils/wxrc
	$(CP_P) $(UTILSDIR)/wxrc/*.rc $(DISTDIR)/utils/wxrc

MISC_DIST: ALL_GUI_DIST

INTL_DIST:
	mkdir $(DISTDIR)/locale
	$(CP_P) $(INTLDIR)/Makefile $(DISTDIR)/locale
	$(CP_P) $(INTLDIR)/*.po $(DISTDIR)/locale
	-$(CP_P) $(INTLDIR)/*.mo $(DISTDIR)/locale
	subdirs=`cd $(INTLDIR) && ls */*.po | sed 's|/.*||' | uniq`; \
	for dir in "$$subdirs"; do                                   \
	    mkdir $(DISTDIR)/locale/$$dir;                           \
	    $(CP_P) $(INTLDIR)/$$dir/*.[pm]o $(DISTDIR)/locale/$$dir;     \
	done

MANUAL_DIST:
	mkdir $(DISTDIR)/docs
	mkdir $(DISTDIR)/docs/latex
	mkdir $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.tex $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.inc $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.gif $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.ini $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.bib $(DISTDIR)/docs/latex/wx
	$(CP_P) $(DOCDIR)/latex/wx/*.sty $(DISTDIR)/docs/latex/wx


# Copy all the files from wxPython needed for the Debian source package,
# and then remove some that are not needed.
PYTHON_DIST:
	for dir in `grep -v '#' $(WXDIR)/wxPython/distrib/DIRLIST`; do \
		echo "Copying dir: $$dir..."; \
		mkdir $(DISTDIR)/$$dir; \
		$(CP_P) $(WXDIR)/$$dir/* $(DISTDIR)/$$dir > /dev/null 2>&1; \
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
			-print | egrep -v '/(samples|dbbrowse)/.*\.hh.$$' | xargs rm -rf

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
	$(CP_P) $(INCDIR)/wx/msw/*.cur _dist_dir/wxMSW/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.ico _dist_dir/wxMSW/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.bmp _dist_dir/wxMSW/include/wx/msw
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.cur
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.ico
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.bmp

debian-dist: DEBIAN_SOURCE_DIR = $(WXDIR)/../wxwidgets@WX_RELEASE@@WX_FLAVOUR@-@WX_SUBVERSION@
debian-dist: debian-native-dist debian-msw-dirs MSW_DIST
	mkdir $(DISTDIR)/debian
	-$(CP_P) $(WXDIR)/debian/* $(DISTDIR)/debian
	$(CP_P) $(DOCDIR)/licence.txt $(DISTDIR)/docs
	$(CP_P) $(DOCDIR)/licendoc.txt $(DISTDIR)/docs
	$(CP_P) $(DOCDIR)/preamble.txt $(DISTDIR)/docs
	rm -f $(DISTDIR)/*.spec

	@# now prune away a lot of the crap included by using cp -R
	@# in other dist targets.  Ugly and hardly portable but it
	@# will run on any Debian box and that's enough for now.

	rm -rf $(DISTDIR)/contrib/build
	find $(DISTDIR) \( -name "CVS" -o -name ".cvsignore" -o -name "*.dsp"    \
			   -o -name "*.dsw" -o -name "*.hh*" -o -name "*.mms"    \
			   -o -name "*.mcp" -o -name "*M*.xml" -o -name "*.r"    \
			   -o -name "*.pro"  \
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
