#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = isosurf
OBJECTS = $(PROGRAM).obj
#EXTRALIBS=$(WXDIR)\lib\glcanvas.lib
#EXTRACPPFLAGS=-I$(WXDIR)\utils\glcanvas\win
EXTRATARGETS=isosurf.dat

!include $(WXDIR)\src\makeprog.wat

isosurf.dat:    isosurf.dat.gz
    gzip -c -d isosurf.dat.gz > isosurf.dat

