##############################################################################
# Name:			src/zlib/makefile.sc
# Purpose:		build zlib Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..
include $(WXDIR)\src\makesc.env
LIBTARGET=$(WXDIR)\lib\zlib$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\src\zlib

# variables
OBJECTS = \
        $(THISDIR)\adler32.obj \
        $(THISDIR)\compress.obj \
        $(THISDIR)\crc32.obj \
        $(THISDIR)\gzio.obj \
        $(THISDIR)\uncompr.obj \
        $(THISDIR)\deflate.obj \
        $(THISDIR)\trees.obj \
        $(THISDIR)\zutil.obj \
        $(THISDIR)\inflate.obj \
        $(THISDIR)\infblock.obj \
        $(THISDIR)\inftrees.obj \
        $(THISDIR)\infcodes.obj \
        $(THISDIR)\infutil.obj \
        $(THISDIR)\inffast.obj

include $(WXDIR)\src\makelib.sc

