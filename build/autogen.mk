# Created: 2005/03/12
# Author: David Elliott

# Usage Example:
#  make -f build/autogen.mk ACLOCAL=aclocal-1.9

# This is a simple Makefile to update the UNIX build system in such a
# way that doing a cvs diff on its output files should reveal only the
# true changes, not meaningless differences due to slightly different
# autoconf or aclocal m4 files.

# For aclocal: All necessary m4 files are located in the build/aclocal
# directory.  Running aclocal -I build/aclocal when using aclocal 1.9
# will result in an aclocal.m4 which uses m4_include for these files.

ACLOCAL=aclocal
AUTOCONF=autoconf
BAKEFILE_GEN=bakefile_gen
AUTOHACKS_PREPEND_INCLUDE_DIR=build/autoconf_prepend-include

# configure depends on everything else so this will build everything.
.PHONY: all
all: configure

.PHONY: autoconf_m4f

# Invoke make on wxAutohacks dir, but don't fail if it's not present
autoconf_m4f:
	-make -C $(AUTOHACKS_PREPEND_INCLUDE_DIR)

BAKEFILES=\
  build/bakefiles/wx.bkl \
  build/bakefiles/common.bkl \
  build/bakefiles/files.bkl \
  build/bakefiles/monolithic.bkl \
  build/bakefiles/multilib.bkl \
  build/bakefiles/opengl.bkl \
  build/bakefiles/plugins.bkl \
  build/bakefiles/build_cfg.bkl

# Run bakefile-gen (which generates everything) whenever a bakefile is newer
# than Makefile.in or autoconf_inc.m4.
# This dep is obviously wrong but probably close enough
autoconf_inc.m4 Makefile.in: $(BAKEFILES)
	cd build/bakefiles && \
	$(BAKEFILE_GEN) -f autoconf

# Run configure whenever configure.in, aclocal.m4 or autoconf_inc.m4 is updated
# Depend on our custom autoconf.m4f
configure: configure.in aclocal.m4 autoconf_inc.m4 autoconf_m4f
	$(AUTOCONF) -B $(AUTOHACKS_PREPEND_INCLUDE_DIR)

ACLOCAL_SOURCES = \
  build/aclocal/bakefile.m4 \
  build/aclocal/bakefile-lang.m4 \
  build/aclocal/cppunit.m4 \
  build/aclocal/gst-element-check.m4 \
  build/aclocal/gtk-2.0.m4 \
  build/aclocal/gtk.m4 \
  build/aclocal/pkg.m4 \
  build/aclocal/sdl.m4

# Run aclocal whenever acinclude or one of our local m4s is updated.
aclocal.m4: configure.in acinclude.m4 $(ACLOCAL_SOURCES)
	$(ACLOCAL) -I build/aclocal

