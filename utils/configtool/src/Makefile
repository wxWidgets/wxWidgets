#
# File:		Makefile for wxconfigtool
# Author:	Julian Smart
# Created:	2002.05.12
# Updated:	
# Copyright:	(c) Julian Smart
#
# To make absolutely everything:
#
# % mkdir /tmp/wxconfigtool-build
# % cd /tmp/wxconfigtool-build
# % make -f $(APPDIR)/src/Makefile WXDIR=$(WXDIR) WXBUILDDIR=$(WXBUILDDIR) APPDIR=$(APPDIR) LEVEL=debug full
#
# To make just ConfigTool:
#
# % make ... LEVEL=debug wxconfigtool
#
# To make wxWindows library:
#
# % make ... LEVEL=debug wx
#
# The result will be subdirectories under your /tmp/wxconfigtool-build (or other specified
# directory) containing:
#
# wxconfigtool-build-debug/
# wxwin-build-debug/
#
# To make a release version, specify LEVEL=release instead of LEVEL=debug.
#

# Current (build) directory
CURRDIR = $(shell pwd)
BUILDDIR=$(CURRDIR)

top_srcdir = $(WXDIR)
top_builddir = $(WXBUILDDIR)
program_dir = 

# If we don't specify a location for wxWindows to be
# built, use a default.
ifeq "$(top_builddir)" ""
topbuilddir = $(CURRDIR)/wxwin-build-$(LEVEL)
WXBUILDDIR=$(top_builddir)
endif 

# ConfigTool build directory
APPBUILDDIR=$(BUILDDIR)/wxconfigtool-build-$(LEVEL)

# StartingBlocks source hierarchy
# You have to specify this when invoking make.
APPDIR=You_should_override_the_APPDIR_variable

# ConfigTool source directory
APPSRCDIR=$(APPDIR)/src

# ConfigTool include directory
APPINCDIR=$(APPDIR)/src

# Extra includes
APPEXTRADEFS= \
  -I$(WXDIR)/contrib/include \
  -I$(APPSRCDIR) \

#  -I$(UTILSDIR) \

WXVERSION=2.5

# Use the small C++ support library
APPEXTRALIBS=-lsupc++

# ${top_builddir}/lib/libwx_$(WXTOOLKIT)$(WXDEBUGSUFFIX)_xrc-$(WXVERSION).a

PLATFORM_OS=$(shell echo $(OS))

ifeq "$(PLATFORM_OS)" "Windows_NT"
ARCHITECTURE="i686"
WXTOOLKIT=msw
else
ARCHITECTURE=$(shell arch)
ifeq "$(ARCHITECTURE)" "i686"
WXTOOLKIT=gtk
else
WXTOOLKIT=mac
endif
endif

#printit:
#	@echo $(PLATFORM_OS)

ifeq "$(LEVEL)" "debug"
WXDEBUGSUFFIX=d
else
WXDEBUGSUFFIX=
endif


# Where to find the wx-config file
WXCONFIG=$(WXBUILDDIR)/wx-config $(WXCONFIGFLAGS)

ifeq "$(LEVEL)" "debug"
WXDEBUGFLAGS=--enable-debug --enable-debug_gdb --enable-debug_cntxt
endif 

PROGRAM=$(APPBUILDDIR)/wxconfigtool

OBJECTS = \
 $(APPBUILDDIR)/appsettings.o \
 $(APPBUILDDIR)/configitem.o \
 $(APPBUILDDIR)/configitemselector.o \
 $(APPBUILDDIR)/configtooldoc.o \
 $(APPBUILDDIR)/configtoolview.o \
 $(APPBUILDDIR)/configtree.o \
 $(APPBUILDDIR)/custompropertydialog.o \
 $(APPBUILDDIR)/htmlparser.o \
 $(APPBUILDDIR)/mainframe.o \
 $(APPBUILDDIR)/propeditor.o \
 $(APPBUILDDIR)/property.o \
 $(APPBUILDDIR)/settingsdialog.o \
 $(APPBUILDDIR)/utils.o \
 $(APPBUILDDIR)/wxconfigtool.o
 
DEPFILES=$(PROGRAM).d

ifeq "$(USEDUMMYMAKEPROG)" "1"
include $(APPSRCDIR)/makeprog.env
else
include $(top_builddir)/src/makeprog.env
endif

# Need to place -lsupc++ at end of libs or we get
# link errors. Note the := syntax to prevent a
# recursive variable error.
LDLIBS:=${LDLIBS} -lsupc++

# Only omit C++ libs for economy of size on Unix
ifeq "$(WXTOOLKIT)" "msw"
CC=c++
CXX=c++
else
CC=gcc
CXX=gcc
endif

mkbuilddir:
	mkdir -p $(APPBUILDDIR)

full: wx all

# --with-libpng=builtin --with-zlib=builtin --with-libjpeg=builtin 
wx:
	@mkdir -p $(WXBUILDDIR) ; \
	cd $(WXBUILDDIR) ; \
        if test ! -f Makefile ; then rm -f *.o *.d ; \
        rm -f *.cache ; \
        $(WXDIR)/configure --disable-shared --enable-gui --enable-log $(WXDEBUGFLAGS) --with-threads --disable-no_rtti --without-sockets --without-odbc --with-libjpeg --with-libpng=builtin --with-zlib=builtin --enable-wave ; \
        fi; \
        make; cd contrib/src/xrc; make

cleanapp: clean

cleanwx:
	rm -f -r $(WXBUILDDIR)

cleanall:	cleanapp cleanwx

wxconfigtool:	mkbuilddir all

# Making a binary, RH Linux tarball release using the setup script.
# It will put the result in /tmp/wxconfigtool-deliver.
release:
	echo "#include <stdio.h>" > /tmp/appver.c
	echo "#include \"symbols.h\"" >> /tmp/appver.c
	echo "int main() { printf(\"%.2f\", ctVERSION_NUMBER); }" >> /tmp/appver.c
	cc /tmp/appver.c -I$(APPSRCDIR) -o /tmp/appver
	chmod a+x $(APPSRCDIR)/../scripts/maketarball.sh
	$(APPSRCDIR)/../scripts/maketarball.sh $(APPDIR) $(APPBUILDDIR) /tmp/wxconfigtool-deliver `/tmp/appver` # --upx
	echo The release is in /tmp/wxconfigtool-deliver.
#	rm -f /tmp/appver /tmp/appver.c

# Gives a list of targets
help:
	@echo "Targets: full, wxconfigtool, wx, cleanall, clean, cleanwx, release"

### File dependencies

# Main app files

$(APPBUILDDIR)/wxconfigtool.o: $(APPSRCDIR)/wxconfigtool.cpp $(APPINCDIR)/wxconfigtool.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/appsettings.o: $(APPSRCDIR)/appsettings.cpp $(APPSRCDIR)/appsettings.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/configitem.o: $(APPSRCDIR)/configitem.cpp $(APPSRCDIR)/configitem.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/configitemselector.o: $(APPSRCDIR)/configitemselector.cpp $(APPSRCDIR)/configitemselector.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/configtooldoc.o: $(APPSRCDIR)/configtooldoc.cpp $(APPSRCDIR)/configtooldoc.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/configtoolview.o: $(APPSRCDIR)/configtoolview.cpp $(APPSRCDIR)/configtoolview.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/configtree.o: $(APPSRCDIR)/configtree.cpp $(APPSRCDIR)/configtree.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/custompropertydialog.o: $(APPSRCDIR)/custompropertydialog.cpp $(APPSRCDIR)/custompropertydialog.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/htmlparser.o: $(APPSRCDIR)/htmlparser.cpp $(APPSRCDIR)/htmlparser.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/mainframe.o: $(APPSRCDIR)/mainframe.cpp $(APPINCDIR)/mainframe.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/propeditor.o: $(APPSRCDIR)/propeditor.cpp $(APPINCDIR)/propeditor.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/property.o: $(APPSRCDIR)/property.cpp $(APPINCDIR)/property.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/settingsdialog.o: $(APPSRCDIR)/settingsdialog.cpp $(APPINCDIR)/settingsdialog.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/utils.o: $(APPSRCDIR)/utils.cpp $(APPINCDIR)/utils.h $(APPSRCDIR)/symbols.h
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

$(APPBUILDDIR)/wxconfigtool_resources.o: $(APPSRCDIR)/wxconfigtool.rc
	$(RESCOMP) -i $< -o $@ $(RESFLAGS) --include-dir $(APPSRCDIR)

