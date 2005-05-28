##############################################################################
# Name:			src/regex/makefile.sc
# Purpose:		build regex Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..

include $(WXDIR)\src\makesc.env

LIBTARGET=$(WXDIR)\lib\regex$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\src\regex

# variables
OBJECTS = \
        $(THISDIR)\regcomp.obj \
        $(THISDIR)\regexec.obj \
        $(THISDIR)\regerror.obj \
        $(THISDIR)\regfree.obj \

include $(WXDIR)\src\makelib.sc

