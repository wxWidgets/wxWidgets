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

# configure depends on everything else so this will build everything.
.PHONY: all
all: configure

BAKEFILES=\
  build/bakefiles/build_cfg.bkl \
  build/bakefiles/common.bkl \
  build/bakefiles/common_samples.bkl \
  build/bakefiles/common_samples_intree.bkl \
  build/bakefiles/common_samples_outoftree.bkl \
  build/bakefiles/config.bkl \
  build/bakefiles/expat.bkl \
  build/bakefiles/files.bkl \
  build/bakefiles/jpeg.bkl \
  build/bakefiles/lexilla.bkl \
  build/bakefiles/mac_bundles.bkl \
  build/bakefiles/monolithic.bkl \
  build/bakefiles/multilib.bkl \
  build/bakefiles/opengl.bkl \
  build/bakefiles/plugins.bkl \
  build/bakefiles/plugins_deps.bkl \
  build/bakefiles/png.bkl \
  build/bakefiles/regex.bkl \
  build/bakefiles/scintilla.bkl \
  build/bakefiles/tiff.bkl \
  build/bakefiles/version.bkl \
  build/bakefiles/wx.bkl \
  build/bakefiles/wxpresets/libsample/libsample.bkl \
  build/bakefiles/wxpresets/presets/wx.bkl \
  build/bakefiles/wxpresets/presets/wx_unix.bkl \
  build/bakefiles/wxpresets/presets/wx_win32.bkl \
  build/bakefiles/wxpresets/presets/wx_xrc.bkl \
  build/bakefiles/zlib.bkl \
  tests/test.bkl

# Run bakefile-gen (which generates everything) whenever a bakefile is newer
# than Makefile.in or autoconf_inc.m4.
# This dep is obviously wrong but probably close enough
autoconf_inc.m4 Makefile.in: $(BAKEFILES)
	cd build/bakefiles && \
	$(BAKEFILE_GEN) -f autoconf

# Run configure whenever configure.ac, aclocal.m4 or autoconf_inc.m4 is updated
configure: configure.ac aclocal.m4 autoconf_inc.m4
	$(AUTOCONF)

ACLOCAL_SOURCES = \
  build/aclocal/ac_raf_func_which_getservbyname_r.m4 \
  build/aclocal/atomic_builtins.m4 \
  build/aclocal/ax_func_which_gethostbyname_r.m4 \
  build/aclocal/bakefile-lang.m4 \
  build/aclocal/bakefile.m4 \
  build/aclocal/pkg.m4 \
  build/aclocal/sdl.m4 \
  build/aclocal/visibility.m4

# Run aclocal whenever acinclude or one of our local m4s is updated.
aclocal.m4: configure.ac acinclude.m4 $(ACLOCAL_SOURCES)
	$(ACLOCAL) -I build/aclocal

