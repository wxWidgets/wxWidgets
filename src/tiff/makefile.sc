##############################################################################
# Name:			src/tiff/makefile.sc
# Purpose:		build tiff Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..

include $(WXDIR)\src\makesc.env

LIBTARGET=$(WXDIR)\lib\tiff$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\src\tiff

# variables
OBJECTS = \
        $(THISDIR)\tif_aux.obj \
		$(THISDIR)\tif_close.obj \
		$(THISDIR)\tif_codec.obj \
		$(THISDIR)\tif_compress.obj \
		$(THISDIR)\tif_dir.obj \
		$(THISDIR)\tif_dirinfo.obj \
		$(THISDIR)\tif_dirread.obj \
		$(THISDIR)\tif_dirwrite.obj \
		$(THISDIR)\tif_dumpmode.obj \
		$(THISDIR)\tif_error.obj \
		$(THISDIR)\tif_fax3.obj \
		$(THISDIR)\tif_fax3sm.obj \
		$(THISDIR)\tif_flush.obj \
		$(THISDIR)\tif_getimage.obj \
		$(THISDIR)\tif_jpeg.obj \
		$(THISDIR)\tif_luv.obj \
		$(THISDIR)\tif_lzw.obj \
		$(THISDIR)\tif_next.obj \
		$(THISDIR)\tif_open.obj \
		$(THISDIR)\tif_packbits.obj \
		$(THISDIR)\tif_pixarlog.obj \
		$(THISDIR)\tif_predict.obj \
		$(THISDIR)\tif_print.obj \
		$(THISDIR)\tif_read.obj \
		$(THISDIR)\tif_strip.obj \
		$(THISDIR)\tif_swab.obj \
		$(THISDIR)\tif_thunder.obj \
		$(THISDIR)\tif_tile.obj \
		$(THISDIR)\tif_version.obj \
		$(THISDIR)\tif_warning.obj \
		$(THISDIR)\tif_win32.obj \
		$(THISDIR)\tif_write.obj \
		$(THISDIR)\tif_zip.obj \

include $(WXDIR)\src\makelib.sc

