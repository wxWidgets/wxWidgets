##############################################################################
# Name:			src/zlib/makefile.sc
# Purpose:		build zlib Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..\..
include $(WXDIR)\src\makesc.env
LIBTARGET=$(WXDIR)\lib\dcsvg$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\contrib\src\svg

# variables
OBJECTS = \
        $(THISDIR)\dcsvg.obj 

include $(WXDIR)\src\makelib.sc

