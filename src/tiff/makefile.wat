#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
#
# Makefile : Builds TIFF library for Watcom C++, WIN32
# This makefile has to rename files because apparently Watcom C++
# can't deal with greater than 8.3 filenames (can't
# make tif_close.c, for example)

WXDIR = ..\..
EXTRACPPFLAGS=-i=..\zlib

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET   = $(WXLIB)\tiff.lib

OBJECTS= &
		_aux.obj &
		close.obj &
		codec.obj &
		compress.obj &
		dir.obj &
		dirinfo.obj &
		dirread.obj &
		dirwrite.obj &
		dumpmode.obj &
		error.obj &
		fax3.obj &
		fax3sm.obj &
		flush.obj &
		getimage.obj &
		jpeg.obj &
		luv.obj &
		lzw.obj &
		next.obj &
		open.obj &
		packbits.obj &
		pixarlog.obj &
		predict.obj &
		print.obj &
		read.obj &
		strip.obj &
		swab.obj &
		thunder.obj &
		tile.obj &
		version.obj &
		warning.obj &
		win32.obj &
		write.obj &
		zip.obj

rename: .SYMBOLIC
		copy tif_predict.h tif_pred.h
		copy tif_aux.c _aux.c 
		copy tif_close.c close.c 
		copy tif_codec.c codec.c 
		copy tif_compress.c compress.c 
		copy tif_dir.c dir.c 
		copy tif_dirinfo.c dirinfo.c 
		copy tif_dirread.c dirread.c 
		copy tif_dirwrite.c dirwrite.c 
		copy tif_dumpmode.c dumpmode.c 
		copy tif_error.c error.c 
		copy tif_fax3.c fax3.c 
		copy tif_fax3sm.c fax3sm.c 
		copy tif_flush.c flush.c 
		copy tif_getimage.c getimage.c 
		copy tif_jpeg.c jpeg.c 
		copy tif_luv.c luv.c 
		copy tif_lzw.c lzw.c 
		copy tif_next.c next.c 
		copy tif_open.c open.c 
		copy tif_packbits.c packbits.c 
		copy tif_pixarlog.c pixarlog.c 
		copy tif_predict.c predict.c 
		copy tif_print.c print.c 
		copy tif_read.c read.c 
		copy tif_strip.c strip.c 
		copy tif_swab.c swab.c 
		copy tif_thunder.c thunder.c 
		copy tif_tile.c tile.c 
		copy tif_version.c version.c 
		copy tif_warning.c warning.c 
		copy tif_win32.c win32.c 
		copy tif_write.c write.c 
		copy tif_zip.c zip.c

all:        rename $(OBJECTS) $(LIBTARGET)

$(LIBTARGET) : $(OBJECTS)
    %create tmp.lbc
    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
    wlib /b /c /n /p=512 $^@ @tmp.lbc

clean:   .SYMBOLIC
    -erase *.obj
    -erase $(LIBTARGET)
    -erase *.pch
    -erase *.err
    -erase *.lbc
	-erase tif_pred.h
	-erase _aux.c 
	-erase close.c 
	-erase codec.c 
	-erase compress.c 
	-erase dir.c 
	-erase dirinfo.c 
	-erase dirread.c 
	-erase dirwrite.c 
	-erase dumpmode.c 
	-erase error.c 
	-erase fax3.c 
	-erase fax3sm.c 
	-erase flush.c 
	-erase getimage.c 
	-erase jpeg.c 
	-erase luv.c 
	-erase lzw.c 
	-erase next.c 
	-erase open.c 
	-erase packbits.c 
	-erase pixarlog.c 
	-erase predict.c 
	-erase print.c 
	-erase read.c 
	-erase strip.c 
	-erase swab.c 
	-erase thunder.c 
	-erase tile.c 
	-erase version.c 
	-erase warning.c 
	-erase win32.c 
	-erase write.c 
	-erase zip.c

cleanall:   clean

