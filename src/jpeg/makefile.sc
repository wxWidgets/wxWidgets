##############################################################################
# Name:			src/jpeg/makefile.sc
# Purpose:		build jpeg Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################

WXDIR = ..\..

include $(WXDIR)\src\makesc.env

LIBTARGET=$(WXDIR)\lib\jpeg$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\src\jpeg

# variables
OBJECTS = \
        $(THISDIR)\jcomapi.obj \
        $(THISDIR)\jutils.obj \
        $(THISDIR)\jerror.obj \
        $(THISDIR)\jmemmgr.obj \
        $(THISDIR)\jmemnobs.obj \
        $(THISDIR)\jcapimin.obj \
        $(THISDIR)\jcapistd.obj \
        $(THISDIR)\jctrans.obj \
        $(THISDIR)\jcparam.obj \
        $(THISDIR)\jdatadst.obj \
        $(THISDIR)\jcinit.obj \
        $(THISDIR)\jcmaster.obj \
        $(THISDIR)\jcmarker.obj \
        $(THISDIR)\jcmainct.obj \
        $(THISDIR)\jcprepct.obj \
        $(THISDIR)\jccoefct.obj \
        $(THISDIR)\jccolor.obj \
        $(THISDIR)\jcsample.obj \
        $(THISDIR)\jchuff.obj \
        $(THISDIR)\jcphuff.obj \
        $(THISDIR)\jcdctmgr.obj \
        $(THISDIR)\jfdctfst.obj \
        $(THISDIR)\jfdctflt.obj \
        $(THISDIR)\jfdctint.obj \
        $(THISDIR)\jdapimin.obj \
        $(THISDIR)\jdapistd.obj \
        $(THISDIR)\jdtrans.obj \
        $(THISDIR)\jdatasrc.obj \
        $(THISDIR)\jdmaster.obj \
        $(THISDIR)\jdinput.obj \
        $(THISDIR)\jdmarker.obj \
        $(THISDIR)\jdhuff.obj \
        $(THISDIR)\jdphuff.obj \
        $(THISDIR)\jdmainct.obj \
        $(THISDIR)\jdcoefct.obj \
        $(THISDIR)\jdpostct.obj \
        $(THISDIR)\jddctmgr.obj \
        $(THISDIR)\jidctfst.obj \
        $(THISDIR)\jidctflt.obj \
        $(THISDIR)\jidctint.obj \
        $(THISDIR)\jidctred.obj \
        $(THISDIR)\jdsample.obj \
        $(THISDIR)\jdcolor.obj \
        $(THISDIR)\jquant1.obj \
        $(THISDIR)\jquant2.obj \
        $(THISDIR)\jdmerge.obj 

include $(WXDIR)\src\makelib.sc

