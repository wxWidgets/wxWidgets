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
RIBBONDIR = $(WXDIR)/src/ribbon
PROPGRIDDIR = $(WXDIR)/src/propgrid
STCDIR =   $(WXDIR)/src/stc
UNIXDIR  = $(WXDIR)/src/unix
PNGDIR   = $(WXDIR)/src/png
JPEGDIR  = $(WXDIR)/src/jpeg
TIFFDIR  = $(WXDIR)/src/tiff
ZLIBDIR  = $(WXDIR)/src/zlib
EXPATDIR = $(WXDIR)/src/expat
GTKDIR   = $(WXDIR)/src/gtk
X11DIR   = $(WXDIR)/src/x11
X11INC   = $(WXDIR)/include/wx/x11
MSWDIR   = $(WXDIR)/src/msw
MACDIR   = $(WXDIR)/src/osx
COCOADIR = $(WXDIR)/src/cocoa
FTDIR    = $(WXDIR)/src/freetype
INCDIR   = $(WXDIR)/include
IFACEDIR = $(WXDIR)/interface
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
BASEDISTDIR=./_dist_dir/wxBase-$(WX_VERSION)

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
	$(CP_P) $(DOCDIR)/lgpl.txt $(DISTDIR)/COPYING.LIB
	$(CP_P) $(DOCDIR)/licence.txt $(DISTDIR)/LICENCE.txt
	$(CP_P) $(DOCDIR)/changes.txt $(DISTDIR)/CHANGES.txt
	mkdir $(DISTDIR)/lib
	$(CP_P) $(WXDIR)/lib/vms.opt $(DISTDIR)/lib
	$(CP_P) $(WXDIR)/lib/vms_gtk.opt $(DISTDIR)/lib
	mkdir $(DISTDIR)/src
	mkdir $(DISTDIR)/src/xml
	$(CP_P) $(SRCDIR)/xml/*.cpp $(DISTDIR)/src/xml
	mkdir $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/*.h $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/*.c $(DISTDIR)/src/zlib
	$(CP_P) $(ZLIBDIR)/README $(DISTDIR)/src/zlib
	#$(CP_P) $(ZLIBDIR)/*.mms $(DISTDIR)/src/zlib
	$(CP_PR) $(EXPATDIR) $(DISTDIR)/src/expat
	#(cd $(DISTDIR)/src/expat ; rm -rf `find -name CVS`)
	# copy some files from include/ that are not installed:
	mkdir $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx
	mkdir $(DISTDIR)/include/wx/private
	$(CP_P) $(INCDIR)/wx/fmappriv.h $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/private/*.h $(DISTDIR)/include/wx/private
	# copy wxpresets
	mkdir $(DISTDIR)/build
	mkdir $(DISTDIR)/build/bakefiles
	$(CP_P) $(WXDIR)/build/bakefiles/README $(DISTDIR)/build/bakefiles
	$(CP_P) $(WXDIR)/build/bakefiles/*.* $(DISTDIR)/build/bakefiles
	mkdir $(DISTDIR)/build/bakefiles/wxpresets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/presets
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/*.txt $(DISTDIR)/build/bakefiles/wxpresets
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/presets/*.bkl $(DISTDIR)/build/bakefiles/wxpresets/presets
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/presets/wx_presets.py $(DISTDIR)/build/bakefiles/wxpresets/presets
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/sample/minimal* $(DISTDIR)/build/bakefiles/wxpresets/sample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/sample/config* $(DISTDIR)/build/bakefiles/wxpresets/sample
	mkdir $(DISTDIR)/build/bakefiles/wxpresets/libsample
	$(CP_P) $(WXDIR)/build/bakefiles/wxpresets/libsample/*.* $(DISTDIR)/build/bakefiles/wxpresets/libsample
	mkdir $(DISTDIR)/build/aclocal
	$(CP_P) $(WXDIR)/build/aclocal/*.m4 $(DISTDIR)/build/aclocal

# this target is the common part of distribution script for all GUI toolkits,
# but is not used when building wxBase distribution
ALL_GUI_DIST: ALL_DIST
	$(CP_P) $(DOCDIR)/readme.txt $(DISTDIR)/README.txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/changes.txt ; then \
	  $(CP_P) $(DOCDIR)/$(TOOLKITDIR)/changes.txt $(DISTDIR)/CHANGES-$(TOOLKIT).txt ; fi
	$(CP_P) $(DOCDIR)/$(TOOLKITDIR)/readme.txt $(DISTDIR)/README-$(TOOLKIT).txt
	if test -f $(DOCDIR)/$(TOOLKITDIR)/todo.txt ; then \
	  $(CP_P) $(DOCDIR)/$(TOOLKITDIR)/todo.txt $(DISTDIR)/TODO.txt ; fi
	mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)
	if test -d $(INCDIR)/wx/$(TOOLKITDIR)/private; then \
	    mkdir $(DISTDIR)/include/wx/$(TOOLKITDIR)/private && \
	    $(CP_P) $(INCDIR)/wx/$(TOOLKITDIR)/private/*.h $(DISTDIR)/include/wx/$(TOOLKITDIR)/private; \
	fi
	mkdir $(DISTDIR)/include/wx/meta
	mkdir $(DISTDIR)/include/wx/generic
	mkdir $(DISTDIR)/include/wx/generic/private
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/richtext
	mkdir $(DISTDIR)/include/wx/richtext/bitmaps
	mkdir $(DISTDIR)/include/wx/aui
	mkdir $(DISTDIR)/include/wx/ribbon
	mkdir $(DISTDIR)/include/wx/persist
	mkdir $(DISTDIR)/include/wx/propgrid
	mkdir $(DISTDIR)/include/wx/stc
	mkdir $(DISTDIR)/include/wx/protocol
	mkdir $(DISTDIR)/include/wx/unix
	mkdir $(DISTDIR)/include/wx/unix/private
	mkdir $(DISTDIR)/include/wx/xml
	mkdir $(DISTDIR)/include/wx/xrc
	$(CP_P) $(INCDIR)/wx/*.h $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/*.cpp $(DISTDIR)/include/wx
	$(CP_P) $(INCDIR)/wx/meta/*.h $(DISTDIR)/include/wx/meta
	$(CP_P) $(INCDIR)/wx/generic/*.h $(DISTDIR)/include/wx/generic
	$(CP_P) $(INCDIR)/wx/generic/private/*.h $(DISTDIR)/include/wx/generic/private
	$(CP_P) $(INCDIR)/wx/html/*.h $(DISTDIR)/include/wx/html
	$(CP_P) $(INCDIR)/wx/richtext/*.h $(DISTDIR)/include/wx/richtext
	$(CP_P) $(INCDIR)/wx/richtext/bitmaps/*.xpm $(DISTDIR)/include/wx/richtext/bitmaps
	$(CP_P) $(INCDIR)/wx/aui/*.h $(DISTDIR)/include/wx/aui
	$(CP_P) $(INCDIR)/wx/ribbon/*.h $(DISTDIR)/include/wx/ribbon
	$(CP_P) $(INCDIR)/wx/persist/*.h $(DISTDIR)/include/wx/persist
	$(CP_P) $(INCDIR)/wx/propgrid/*.h $(DISTDIR)/include/wx/propgrid
	$(CP_P) $(INCDIR)/wx/stc/*.h $(DISTDIR)/include/wx/stc
	$(CP_P) $(INCDIR)/wx/protocol/*.h $(DISTDIR)/include/wx/protocol
	$(CP_P) $(INCDIR)/wx/unix/*.h $(DISTDIR)/include/wx/unix
	$(CP_P) $(INCDIR)/wx/unix/private/*.h $(DISTDIR)/include/wx/unix/private
	$(CP_P) $(INCDIR)/wx/xml/*.h $(DISTDIR)/include/wx/xml
	$(CP_P) $(INCDIR)/wx/xrc/*.h $(DISTDIR)/include/wx/xrc

	mkdir $(DISTDIR)/art
	mkdir $(DISTDIR)/art/gtk
	$(CP_P) $(WXDIR)/art/*.xpm $(DISTDIR)/art
	$(CP_P) $(WXDIR)/art/gtk/*.xpm $(DISTDIR)/art/gtk

	mkdir $(DISTDIR)/src/$(TOOLKITDIR)
	$(CP_P) $(COMMDIR)/*.cpp $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.c $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.inc $(DISTDIR)/src/common
	$(CP_P) $(COMMDIR)/*.mms $(DISTDIR)/src/common

	mkdir $(DISTDIR)/src/xrc
	$(CP_P) $(SRCDIR)/xrc/*.cpp $(DISTDIR)/src/xrc

	mkdir $(DISTDIR)/src/unix
	$(CP_P) $(UNIXDIR)/*.cpp $(DISTDIR)/src/unix
	$(CP_P) $(UNIXDIR)/*.mms $(DISTDIR)/src/unix

	mkdir $(DISTDIR)/src/generic
	$(CP_P) $(GENDIR)/*.cpp $(DISTDIR)/src/generic
	$(CP_P) $(GENDIR)/*.mms $(DISTDIR)/src/generic

	mkdir $(DISTDIR)/src/html
	$(CP_P) $(HTMLDIR)/*.cpp $(DISTDIR)/src/html

	mkdir $(DISTDIR)/src/richtext
	$(CP_P) $(RICHTEXTDIR)/*.cpp $(DISTDIR)/src/richtext

	mkdir $(DISTDIR)/src/aui
	$(CP_P) $(AUIDIR)/*.cpp $(DISTDIR)/src/aui

	mkdir $(DISTDIR)/src/ribbon
	$(CP_P) $(RIBBONDIR)/*.cpp $(DISTDIR)/src/ribbon

	mkdir $(DISTDIR)/src/propgrid
	$(CP_P) $(PROPGRIDDIR)/*.cpp $(DISTDIR)/src/propgrid

	mkdir $(DISTDIR)/src/stc
	mkdir $(DISTDIR)/src/stc/scintilla
	mkdir $(DISTDIR)/src/stc/scintilla/src
	mkdir $(DISTDIR)/src/stc/scintilla/include
	$(CP_P) $(STCDIR)/*.* $(DISTDIR)/src/stc
	$(CP_P) $(STCDIR)/scintilla/src/* $(DISTDIR)/src/stc/scintilla/src
	$(CP_P) $(STCDIR)/scintilla/include/* $(DISTDIR)/src/stc/scintilla/include

	mkdir $(DISTDIR)/src/png
	$(CP_PR) $(PNGDIR)/* $(DISTDIR)/src/png

	mkdir $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/*.h $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/*.c $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/*.vc $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/makefile.* $(DISTDIR)/src/jpeg
	$(CP_P) $(JPEGDIR)/README $(DISTDIR)/src/jpeg

	mkdir $(DISTDIR)/src/tiff
	mkdir $(DISTDIR)/src/tiff/config
	mkdir $(DISTDIR)/src/tiff/contrib
	mkdir $(DISTDIR)/src/tiff/port
	mkdir $(DISTDIR)/src/tiff/html
	mkdir $(DISTDIR)/src/tiff/man
	mkdir $(DISTDIR)/src/tiff/libtiff
	mkdir $(DISTDIR)/src/tiff/tools
	mkdir $(DISTDIR)/src/tiff/test
	mkdir $(DISTDIR)/src/tiff/m4
	$(CP_P) $(TIFFDIR)/config/* $(DISTDIR)/src/tiff/config
	$(CP_PR) $(TIFFDIR)/contrib/* $(DISTDIR)/src/tiff/contrib
	$(CP_P) $(TIFFDIR)/port/* $(DISTDIR)/src/tiff/port
	$(CP_PR) $(TIFFDIR)/html/* $(DISTDIR)/src/tiff/html
	$(CP_P) $(TIFFDIR)/man/* $(DISTDIR)/src/tiff/man
	$(CP_P) $(TIFFDIR)/tools/* $(DISTDIR)/src/tiff/tools
	$(CP_P) $(TIFFDIR)/test/* $(DISTDIR)/src/tiff/test
	$(CP_P) $(TIFFDIR)/libtiff/* $(DISTDIR)/src/tiff/libtiff
	$(CP_P) $(TIFFDIR)/m4/* $(DISTDIR)/src/tiff/m4
	$(CP_P) $(TIFFDIR)/README $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/VERSION $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/configure* $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/aclocal.m4 $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/autogen.sh $(DISTDIR)/src/tiff
	$(CP_P) $(TIFFDIR)/Makefile* $(DISTDIR)/src/tiff

BASE_DIST: ALL_DIST INTL_DIST
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
	mkdir $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/html
	mkdir $(DISTDIR)/include/wx/richtext
	mkdir $(DISTDIR)/include/wx/richtext/bitmaps
	mkdir $(DISTDIR)/include/wx/aui
	mkdir $(DISTDIR)/include/wx/ribbon
	mkdir $(DISTDIR)/include/wx/persist
	mkdir $(DISTDIR)/include/wx/propgrid
	mkdir $(DISTDIR)/include/wx/stc
	mkdir $(DISTDIR)/include/wx/osx
	mkdir $(DISTDIR)/include/wx/osx/carbon
	mkdir $(DISTDIR)/include/wx/osx/core
	mkdir $(DISTDIR)/src/unix
	mkdir $(DISTDIR)/src/osx
	mkdir $(DISTDIR)/src/osx/core
	mkdir $(DISTDIR)/src/osx/carbon
	mkdir $(DISTDIR)/src/msw
	$(CP_P) $(DOCDIR)/base/readme.txt $(DISTDIR)/README.txt
	$(CP_P) $(WXDIR)/src/common/*.inc $(DISTDIR)/src/common
	list='$(ALL_PORTS_BASE_HEADERS)'; for p in $$list; do \
	  $(CP_P) $(WXDIR)/include/$$p $(DISTDIR)/include/$$p; \
	done
	list='$(ALL_BASE_SOURCES)'; for p in $$list; do \
	  $(CP_P) $(WXDIR)/$$p $(DISTDIR)/$$p; \
	done

	mkdir $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/Makefile.in $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/makefile.* $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/sample.* $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/samples.* $(DISTDIR)/samples

	mkdir $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/Makefile.in $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/makefile.unx $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/console.cpp $(DISTDIR)/samples/console
	$(CP_P) $(SAMPDIR)/console/console.dsp $(DISTDIR)/samples/console

	mv $(DISTDIR) $(BASEDISTDIR)

GTK_DIST: UNIV_DIST
	$(CP_P) $(INCDIR)/wx/gtk/*.h $(DISTDIR)/include/wx/gtk
	$(CP_P) $(GTKDIR)/*.h $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.cpp $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.c $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.xbm $(DISTDIR)/src/gtk
	$(CP_P) $(GTKDIR)/*.mms $(DISTDIR)/src/gtk
	mkdir $(DISTDIR)/include/wx/x11/private
	$(CP_P) $(INCDIR)/wx/x11/private/*.h $(DISTDIR)/include/wx/x11/private

	mkdir $(DISTDIR)/include/wx/gtk/gnome
	mkdir $(DISTDIR)/src/gtk/gnome
	$(CP_P) $(INCDIR)/wx/gtk/gnome/*.h $(DISTDIR)/include/wx/gtk/gnome
	$(CP_P) $(GTKDIR)/gnome/*.cpp $(DISTDIR)/src/gtk/gnome

	mkdir $(DISTDIR)/src/osx
	mkdir $(DISTDIR)/src/osx/core
	$(CP_P) $(WXDIR)/src/osx/core/*.cpp $(DISTDIR)/src/osx/core
	mkdir $(DISTDIR)/include/wx/osx
	mkdir $(DISTDIR)/include/wx/osx/core
	$(CP_P) $(WXDIR)/include/wx/osx/core/*.h $(DISTDIR)/include/wx/osx/core

X11_DIST: UNIV_DIST
	$(CP_P) $(INCDIR)/wx/x11/*.h $(DISTDIR)/include/wx/x11
	mkdir $(DISTDIR)/include/wx/x11/private
	$(CP_P) $(INCDIR)/wx/x11/private/*.h $(DISTDIR)/include/wx/x11/private
	mkdir $(DISTDIR)/include/wx/gtk/private
	$(CP_P) $(INCDIR)/wx/gtk/private/string.h $(DISTDIR)/include/wx/gtk/private
	$(CP_P) $(X11DIR)/*.cpp $(DISTDIR)/src/x11
	$(CP_P) $(X11DIR)/*.c $(DISTDIR)/src/x11
	$(CP_P) $(X11DIR)/*.xbm $(DISTDIR)/src/x11
	mkdir $(DISTDIR)/src/osx
	mkdir $(DISTDIR)/src/osx/core
	$(CP_P) $(WXDIR)/src/osx/core/*.cpp $(DISTDIR)/src/osx/core
	mkdir $(DISTDIR)/include/wx/osx
	mkdir $(DISTDIR)/include/wx/osx/core
	$(CP_P) $(WXDIR)/include/wx/osx/core/*.h $(DISTDIR)/include/wx/osx/core

OSX_CARBON_DIST: ALL_GUI_DIST
	$(CP_P) $(INCDIR)/*.* $(DISTDIR)/include
	mkdir $(DISTDIR)/include/wx/osx/carbon
	mkdir $(DISTDIR)/include/wx/osx/carbon/private
	mkdir $(DISTDIR)/include/wx/osx/cocoa
	mkdir $(DISTDIR)/include/wx/osx/iphone
	$(CP_P) $(INCDIR)/wx/osx/*.h $(DISTDIR)/include/wx/osx
	$(CP_P) $(INCDIR)/wx/osx/carbon/*.h $(DISTDIR)/include/wx/osx/carbon
	$(CP_P) $(INCDIR)/wx/osx/carbon/private/*.h $(DISTDIR)/include/wx/osx/carbon/private
	$(CP_P) $(INCDIR)/wx/osx/private/*.h $(DISTDIR)/include/wx/osx/private
	$(CP_P) $(INCDIR)/wx/osx/cocoa/*.h $(DISTDIR)/include/wx/osx/cocoa
	$(CP_P) $(INCDIR)/wx/osx/iphone/*.h $(DISTDIR)/include/wx/osx/iphone
	mkdir $(DISTDIR)/include/wx/osx/core
	mkdir $(DISTDIR)/include/wx/osx/core/private
	$(CP_P) $(INCDIR)/wx/osx/core/*.h $(DISTDIR)/include/wx/osx/core
	$(CP_P) $(INCDIR)/wx/osx/core/private/*.h $(DISTDIR)/include/wx/osx/core/private
	mkdir $(DISTDIR)/src/osx/core
	$(CP_P) $(MACDIR)/core/*.cpp $(DISTDIR)/src/osx/core
	mkdir $(DISTDIR)/src/osx/cocoa
	$(CP_P) $(MACDIR)/cocoa/*.mm $(DISTDIR)/src/osx/cocoa
	mkdir $(DISTDIR)/src/osx/iphone
	$(CP_P) $(MACDIR)/iphone/*.mm $(DISTDIR)/src/osx/iphone
	mkdir $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/*.cpp $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/*.mm $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/*.icns $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/*.h $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/*.r $(DISTDIR)/src/osx/carbon
	mkdir $(DISTDIR)/src/wxWindows.xcodeproj
	$(CP_P) $(WXDIR)/src/wxWindows.xcodeproj/* $(DISTDIR)/src/wxWindows.xcodeproj

COCOA_DIST: ALL_GUI_DIST
	$(CP_P) $(INCDIR)/wx/cocoa/*.h $(DISTDIR)/include/wx/cocoa
	$(CP_P) $(COCOADIR)/*.mm $(DISTDIR)/src/cocoa
	$(CP_P) $(COCOADIR)/*.cpp $(DISTDIR)/src/cocoa
	$(CP_P) $(COCOADIR)/*.r $(DISTDIR)/src/cocoa
	mkdir $(DISTDIR)/include/wx/osx/core
	$(CP_P) $(INCDIR)/wx/osx/core/*.h $(DISTDIR)/include/wx/osx/core
	mkdir $(DISTDIR)/src/osx/core
	$(CP_P) $(MACDIR)/core/*.cpp $(DISTDIR)/src/osx/core
	mkdir $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/Info.plist.in $(DISTDIR)/src/osx/carbon
	$(CP_P) $(MACDIR)/carbon/wxmac.icns $(DISTDIR)/src/osx/carbon

MSW_DIST: UNIV_DIST
	mkdir $(DISTDIR)/include/wx/msw/ole
	$(CP_P) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.cur $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.ico $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.bmp $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.manifest $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/src/msw/ole
	$(CP_P) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.rc $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole

MSW_ZIP_TEXT_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/build/msw
	$(CP_P) $(WXDIR)/build/msw/* $(DISTDIR)/build/msw
	$(CP_P) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	mkdir $(DISTDIR)/include/wx/msw/ole
	$(CP_P) $(INCDIR)/wx/msw/*.h $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.rc $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.manifest $(DISTDIR)/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/ole/*.h $(DISTDIR)/include/wx/msw/ole
	mkdir $(DISTDIR)/src/msw/ole
	$(CP_P) $(MSWDIR)/*.cpp $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.rc $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/*.c $(DISTDIR)/src/msw
	$(CP_P) $(MSWDIR)/ole/*.cpp $(DISTDIR)/src/msw/ole
	$(CP_P) $(SRCDIR)/*.??? $(DISTDIR)/src

UNIV_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/include/wx/univ
	mkdir $(DISTDIR)/src/univ
	mkdir $(DISTDIR)/src/univ/themes
	$(CP_P) $(INCDIR)/wx/univ/*.h $(DISTDIR)/include/wx/univ
	$(CP_P) $(SRCDIR)/univ/*.cpp $(DISTDIR)/src/univ
	$(CP_P) $(SRCDIR)/univ/themes/*.cpp $(DISTDIR)/src/univ/themes

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
	$(CP_P) $(SAMPDIR)/makefile.* $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/sample.* $(DISTDIR)/samples
	$(CP_P) $(SAMPDIR)/samples.* $(DISTDIR)/samples

	# copy files common to all samples in a general way (samples without
	# Makefile.in in them are Windows-specific and shouldn't be included in
	# Unix distribution)
	for s in `find $(SAMPDIR) $(SAMPDIR)/html $(SAMPDIR)/opengl \
		    -mindepth 1 -maxdepth 1 -type d -not -name .svn`; do \
	    if [ ! -f $$s/Makefile.in ]; then continue; fi; \
	    t="$(DISTDIR)/samples/`echo $$s | sed 's@$(SAMPDIR)/@@'`"; \
	    mkdir -p $$t; \
	    $(CP_P) $$s/Makefile.in \
		    `find $$s -maxdepth 1 -name 'makefile.*' -o -name descrip.mms -o \
				-name '*.cpp' -o -name '*.h' -o \
				-name '*.bmp' -o -name '*.ico' -o -name '*.png' -o \
				-name '*.rc' -o -name '*.xpm'` $$t; \
	    i=""; \
	    if [ -d $$s/bitmaps ]; then i="bitmaps"; fi; \
	    if [ -d $$s/icons ]; then i="icons"; fi; \
	    if [ -n "$$i" ]; then \
		mkdir $$t/$$i; \
		cp `find $$s/$$i -name '*.bmp' -o -name '*.xpm'` $$t/$$i; \
	    fi; \
	done

	# copy the rest, not covered by the above loop
	$(CP_P) $(SAMPDIR)/animate/hourglass.ani $(DISTDIR)/samples/animate
	$(CP_P) $(SAMPDIR)/animate/throbber.gif $(DISTDIR)/samples/animate

	$(CP_P) $(SAMPDIR)/dialogs/tips.txt $(DISTDIR)/samples/dialogs

	$(CP_P) $(SAMPDIR)/dnd/d_and_d.txt $(DISTDIR)/samples/dnd

	mkdir $(DISTDIR)/samples/help/doc
	$(CP_P) $(SAMPDIR)/help/*.h?? $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.gif $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/cshelp.txt $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.chm $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/*.cnt $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/doc.zip $(DISTDIR)/samples/help
	$(CP_P) $(SAMPDIR)/help/doc/*.* $(DISTDIR)/samples/help/doc

	mkdir $(DISTDIR)/samples/html/about/data
	$(CP_P) $(SAMPDIR)/html/about/data/*.htm $(DISTDIR)/samples/html/about/data
	$(CP_P) $(SAMPDIR)/html/about/data/*.png $(DISTDIR)/samples/html/about/data
	mkdir $(DISTDIR)/samples/html/help/helpfiles
	$(CP_P) $(SAMPDIR)/html/help/helpfiles/*.??? $(DISTDIR)/samples/html/help/helpfiles
	$(CP_P) $(SAMPDIR)/html/helpview/*.zip $(DISTDIR)/samples/html/helpview
	$(CP_P) $(SAMPDIR)/html/printing/*.htm $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/printing/*.gif $(DISTDIR)/samples/html/printing
	$(CP_P) $(SAMPDIR)/html/test/*.gif $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.htm $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/test/*.html $(DISTDIR)/samples/html/test
	$(CP_P) $(SAMPDIR)/html/virtual/*.htm $(DISTDIR)/samples/html/virtual
	$(CP_P) $(SAMPDIR)/html/widget/*.htm $(DISTDIR)/samples/html/widget
	$(CP_P) $(SAMPDIR)/html/zip/*.htm $(DISTDIR)/samples/html/zip
	$(CP_P) $(SAMPDIR)/html/zip/*.zip $(DISTDIR)/samples/html/zip

	$(CP_P) $(SAMPDIR)/image/horse*.* $(DISTDIR)/samples/image
	$(CP_P) $(SAMPDIR)/image/smile.xbm $(DISTDIR)/samples/image

	$(CP_P) $(SAMPDIR)/internat/readme.txt $(DISTDIR)/samples/internat
	for f in `(cd $(SAMPDIR); find internat -name '*.[mp]o' -print)`; do \
	    mkdir -p $(DISTDIR)/samples/`dirname $$f`;  \
	    $(CP_P) $(SAMPDIR)/$$f $(DISTDIR)/samples/$$f; \
	done

	$(CP_P) $(SAMPDIR)/joytest/*.wav $(DISTDIR)/samples/joytest

	$(CP_P) $(SAMPDIR)/opengl/penguin/trackball.c $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/penguin/*.dxf.gz $(DISTDIR)/samples/opengl/penguin
	$(CP_P) $(SAMPDIR)/opengl/isosurf/*.gz $(DISTDIR)/samples/opengl/isosurf

	$(CP_P) $(SAMPDIR)/richtext/readme.txt $(DISTDIR)/samples/richtext
	$(CP_P) $(SAMPDIR)/richtext/todo.txt $(DISTDIR)/samples/richtext

	$(CP_P) $(SAMPDIR)/sound/*.wav $(DISTDIR)/samples/sound

	$(CP_P) $(SAMPDIR)/splash/*.mpg $(DISTDIR)/samples/splash

	mkdir $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.xpm $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.xrc $(DISTDIR)/samples/xrc/rc
	$(CP_P) $(SAMPDIR)/xrc/rc/*.gif $(DISTDIR)/samples/xrc/rc

UTILS_DIST: ALL_GUI_DIST
	mkdir $(DISTDIR)/utils
	$(CP_P) $(UTILSDIR)/Makefile.in $(DISTDIR)/utils

	mkdir $(DISTDIR)/utils/screenshotgen
	mkdir $(DISTDIR)/utils/screenshotgen/src
	mkdir $(DISTDIR)/utils/screenshotgen/src/bitmaps
	$(CP_P) $(UTILSDIR)/screenshotgen/README.txt $(DISTDIR)/utils/screenshotgen
	$(CP_P) $(UTILSDIR)/screenshotgen/*.in $(DISTDIR)/utils/screenshotgen
	$(CP_P) $(UTILSDIR)/screenshotgen/src/*.* $(DISTDIR)/utils/screenshotgen/src
	$(CP_P) $(UTILSDIR)/screenshotgen/src/bitmaps/*.* $(DISTDIR)/utils/screenshotgen/src/bitmaps

	mkdir $(DISTDIR)/utils/ifacecheck
	mkdir $(DISTDIR)/utils/ifacecheck/src
	$(CP_P) $(UTILSDIR)/ifacecheck/README.txt $(DISTDIR)/utils/ifacecheck
	$(CP_P) $(UTILSDIR)/ifacecheck/*.dtd $(DISTDIR)/utils/ifacecheck
	$(CP_P) $(UTILSDIR)/ifacecheck/*.xsl $(DISTDIR)/utils/ifacecheck
	$(CP_P) $(UTILSDIR)/ifacecheck/*.in $(DISTDIR)/utils/ifacecheck
	$(CP_P) $(UTILSDIR)/ifacecheck/src/* $(DISTDIR)/utils/ifacecheck/src

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
	subdirs=`cd $(INTLDIR) && ls */*.po | sed 's|/.*||' | uniq`; \
	for dir in "$$subdirs"; do                                   \
	    mkdir $(DISTDIR)/locale/$$dir;                           \
	    $(CP_P) $(INTLDIR)/$$dir/*.[pm]o $(DISTDIR)/locale/$$dir;     \
	done

MANUAL_DIST:
	mkdir $(DISTDIR)/docs
	mkdir $(DISTDIR)/docs/doxygen
	mkdir $(DISTDIR)/docs/doxygen/groups
	mkdir $(DISTDIR)/docs/doxygen/images
	mkdir $(DISTDIR)/docs/doxygen/images/stock
	mkdir $(DISTDIR)/docs/doxygen/images/wxgtk
	mkdir $(DISTDIR)/docs/doxygen/images/wxmac
	mkdir $(DISTDIR)/docs/doxygen/images/wxmsw
	mkdir $(DISTDIR)/docs/doxygen/mainpages
	mkdir $(DISTDIR)/docs/doxygen/overviews
	$(CP_P) $(DOCDIR)/doxygen/* $(DISTDIR)/docs/doxygen
	$(CP_P) $(DOCDIR)/doxygen/groups/*.h $(DISTDIR)/docs/doxygen/groups
	$(CP_P) $(DOCDIR)/doxygen/mainpages/*.h $(DISTDIR)/docs/doxygen/mainpages
	$(CP_P) $(DOCDIR)/doxygen/overviews/*.h $(DISTDIR)/docs/doxygen/overviews
	$(CP_P) $(DOCDIR)/doxygen/images/*.??? $(DISTDIR)/docs/doxygen/images
	$(CP_P) $(DOCDIR)/doxygen/images/wxgtk/*.??? $(DISTDIR)/docs/doxygen/images/stock
	$(CP_P) $(DOCDIR)/doxygen/images/wxgtk/*.??? $(DISTDIR)/docs/doxygen/images/wxgtk
	$(CP_P) $(DOCDIR)/doxygen/images/wxmac/*.??? $(DISTDIR)/docs/doxygen/images/wxmac
	$(CP_P) $(DOCDIR)/doxygen/images/wxmsw/*.??? $(DISTDIR)/docs/doxygen/images/wxmsw
	mkdir $(DISTDIR)/interface
	mkdir $(DISTDIR)/interface/wx
	mkdir $(DISTDIR)/interface/wx/aui
	mkdir $(DISTDIR)/interface/wx/ribbon
	mkdir $(DISTDIR)/interface/wx/generic
	mkdir $(DISTDIR)/interface/wx/html
	mkdir $(DISTDIR)/interface/wx/msw
	mkdir $(DISTDIR)/interface/wx/msw/ole
	mkdir $(DISTDIR)/interface/wx/persist
	mkdir $(DISTDIR)/interface/wx/protocol
	mkdir $(DISTDIR)/interface/wx/propgrid
	mkdir $(DISTDIR)/interface/wx/richtext
	mkdir $(DISTDIR)/interface/wx/stc
	mkdir $(DISTDIR)/interface/wx/xml
	mkdir $(DISTDIR)/interface/wx/xrc
	$(CP_P) $(IFACEDIR)/wx/*.h $(DISTDIR)/interface/wx
	$(CP_P) $(IFACEDIR)/wx/aui/*.h $(DISTDIR)/interface/wx/aui
	$(CP_P) $(IFACEDIR)/wx/ribbon/*.h $(DISTDIR)/interface/wx/ribbon
	$(CP_P) $(IFACEDIR)/wx/generic/*.h $(DISTDIR)/interface/wx/generic
	$(CP_P) $(IFACEDIR)/wx/html/*.h $(DISTDIR)/interface/wx/html
	$(CP_P) $(IFACEDIR)/wx/msw/*.h $(DISTDIR)/interface/wx/msw
	$(CP_P) $(IFACEDIR)/wx/msw/ole/*.h $(DISTDIR)/interface/wx/msw/ole
	$(CP_P) $(IFACEDIR)/wx/persist/*.h $(DISTDIR)/interface/wx/persist
	$(CP_P) $(IFACEDIR)/wx/protocol/*.h $(DISTDIR)/interface/wx/protocol
	$(CP_P) $(IFACEDIR)/wx/propgrid/*.h $(DISTDIR)/interface/wx/propgrid
	$(CP_P) $(IFACEDIR)/wx/richtext/*.h $(DISTDIR)/interface/wx/richtext
	$(CP_P) $(IFACEDIR)/wx/stc/*.h $(DISTDIR)/interface/wx/stc
	$(CP_P) $(IFACEDIR)/wx/xml/*.h $(DISTDIR)/interface/wx/xml
	$(CP_P) $(IFACEDIR)/wx/xrc/*.h $(DISTDIR)/interface/wx/xrc


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
			\( -name "makefile.*" -a ! -name "makefile.gcc" -a ! -name "makefile.unx" \) \) \
			-print | egrep -v '/samples/.*\.hh.$$' | xargs rm -rf

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

win-dist: MSW_ZIP_TEXT_DIST SAMPLES_DIST DEMOS_DIST UTILS_DIST MISC_DIST INTL_DIST
	for s in `find $(SAMPDIR) $(SAMPDIR)/html $(SAMPDIR)/opengl \
		    -mindepth 1 -maxdepth 1 -type d -not -name CVS`; do \
	    t="$(DISTDIR)/samples/`echo $$s | sed 's@$(SAMPDIR)/@@'`"; \
	    $(CP_P) \
		    `find $$s -maxdepth 1 -name '*.dsp' -o -name '*.vcproj'` $$t; \
	done
# RR: Copy text and binary data separately
	@echo "*** Creating wxWidgets ZIP distribution in $(DISTDIR)..."
	@cd _dist_dir && mv $(DISTDIRNAME) wxMSW
	@cd _dist_dir && zip -r -l  ../$(WXARCHIVE_ZIP) *
	$(CP_P) $(INCDIR)/wx/msw/*.cur _dist_dir/wxMSW/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.ico _dist_dir/wxMSW/include/wx/msw
	$(CP_P) $(INCDIR)/wx/msw/*.bmp _dist_dir/wxMSW/include/wx/msw
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.cur
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.ico
	@cd _dist_dir && zip -r ../$(WXARCHIVE_ZIP) wxMSW/include/wx/msw/*.bmp
