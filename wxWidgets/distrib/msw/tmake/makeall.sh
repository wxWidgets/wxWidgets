#!/bin/sh
#
# File:      makeall.sh
# Purpose:   create wxWindows makefiles for all compilers
# Author:    Michael Bedward
# Created:   29 Aug 1999
# Copyright: (c) 1999 Michael Bedward
# Licence:   wxWindows Licence
# Version:   $Id$
#
# This creates the makefiles for all compilers from the templates using
# tmake. The tmake executable should be in the path.

# Assume we are in distrib/msw/tmake
#
topdir="../../.."
srcdir="$topdir/src"
mswdir="$srcdir/msw"
[ -z $TMAKE ] && TMAKE=./tmake

for tname in `ls *.t`
do
    case $tname in
	b32.t)
	    echo "Generating $mswdir/makefile.b32 for Borland C++ (32 bits)..."
	    $TMAKE -t b32 wxwin.pro -o $mswdir/makefile.b32
	    $TMAKE -t b32base wxwin.pro -o $mswdir/makebase.b32 ;;

	bcc.t)
	    echo "Generating $mswdir/makefile.bcc for Borland C++ (16 bits)..."
	    $TMAKE -t bcc wxwin.pro -o $mswdir/makefile.bcc;;

	g95.t)
	    echo "Generating $mswdir/makefile.g95 for Cygwin/Mingw32..."
	    $TMAKE -t g95 wxwin.pro -o $mswdir/makefile.g95;;

	vc.t)
	    echo "Generating $mswdir/makefile.vc for Visual C++ 4.0..."
	    $TMAKE -t vc wxwin.pro -o $mswdir/makefile.vc;;

	vc6msw.t)
	    echo "Generating $srcdir/wxWindows.dsp for Visual C++ 6.0..."
	    $TMAKE -t vc6msw wxwin.pro -o $srcdir/wxWindows.dsp;;

	vc6base.t)
	    echo "Generating $srcdir/wxBase.dsp for Visual C++ 6.0..."
	    $TMAKE -t vc6base wxwin.pro -o $srcdir/wxBase.dsp;;

	vc6univ.t)
	    echo "Generating $srcdir/wxUniv.dsp for Visual C++ 6.0..."
	    $TMAKE -t vc6univ wxwin.pro -o $srcdir/wxUniv.dsp;;

	wat.t)
	    echo "Generating $mswdir/makefile.wat for Watcom C++..."
	    $TMAKE -t wat wxwin.pro -o $mswdir/makefile.wat;;

	base.t)
	    echo "Generating $topdir/src/files.lst for Configure..."
	    $TMAKE -t base wxwin.pro -o $topdir/src/files.lst ;;

	gtk.t)
	    echo "Generating $topdir/src/gtk/files.lst for GTK and Configure..."
	    $TMAKE -t gtk wxwin.pro -o $topdir/src/gtk/files.lst;;

	mgl.t)
	    echo "Generating $topdir/src/mgl/files.lst for MGL and Configure..."
	    $TMAKE -t mgl wxwin.pro -o $topdir/src/mgl/files.lst;;

	micro.t)
	    echo "Generating $topdir/src/micro/files.lst for MicroWindows and Configure..."
	    $TMAKE -t micro wxwin.pro -o $topdir/src/microwin/files.lst;;

	msw.t)
	    echo "Generating $topdir/src/msw/files.lst for MSW and Configure..."
	    $TMAKE -t msw wxwin.pro -o $topdir/src/msw/files.lst;;

	mac.t)
	    echo "Generating $topdir/src/mac/files.lst for Mac and Configure..."
	    $TMAKE -t mac wxwin.pro -o $topdir/src/mac/files.lst;;

	motif.t)
	    echo "Generating $topdir/src/motif/files.lst for Motif and Configure..."
	    $TMAKE -t motif wxwin.pro -o $topdir/src/motif/files.lst;;

	univ.t)
	    echo "Generating $topdir/src/univ/files.lst for wxUniversal..."
	    $TMAKE -t univ wxwin.pro -o $topdir/src/univ/files.lst;;

	unx.t)
	    echo "Generating $topdir/src/os2/files.lst for OS/2 PM and Configure..."
	    $TMAKE -t os2 wxwin.pro -o $topdir/src/os2/files.lst;;

	mgl.t)
	    echo "Generating $topdir/src/mgl/files.lst for MGL and Configure..."
	    $TMAKE -t mgl wxwin.pro -o $topdir/src/mgl/files.lst;;

	x11.t)
	    echo "Generating $topdir/src/x11/files.lst for X11 and Configure..."
	    $TMAKE -t x11 wxwin.pro -o $topdir/src/x11/files.lst;;

	watmgl.t)
	    echo "Generating $topdir/src/mgl/makefile.wat for Watcom C++ and MGL+DOS..."
	    $TMAKE -t watmgl wxwin.pro -o $topdir/src/mgl/makefile.wat;;
    esac
done

