##############################################################################
# Name:			src/png/makefile.sc
# Purpose:		build png Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..
include $(WXDIR)\src\makesc.env
LIBTARGET=$(WXDIR)\lib\png$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\src\png

# variables
OBJECTS = \
        $(THISDIR)\png.obj \
        $(THISDIR)\pngread.obj \
        $(THISDIR)\pngrtran.obj \
        $(THISDIR)\pngrutil.obj \
        $(THISDIR)\pngpread.obj \
        $(THISDIR)\pngtrans.obj \
        $(THISDIR)\pngwrite.obj \
        $(THISDIR)\pngwtran.obj \
        $(THISDIR)\pngwutil.obj \
        $(THISDIR)\pngerror.obj \
        $(THISDIR)\pngmem.obj \
        $(THISDIR)\pngwio.obj \
        $(THISDIR)\pngrio.obj \
        $(THISDIR)\pngget.obj \
        $(THISDIR)\pngset.obj 


include $(WXDIR)\src\makelib.sc\

