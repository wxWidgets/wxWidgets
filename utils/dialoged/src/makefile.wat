#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = $(OUTPUTDIR)\dialoged
OBJECTS = $(OUTPUTDIR)\dialoged.obj $(OUTPUTDIR)\reseditr.obj $(OUTPUTDIR)\dlghndlr.obj &
    $(OUTPUTDIR)\reswrite.obj $(OUTPUTDIR)\winprop.obj $(OUTPUTDIR)\edtree.obj  &
    $(OUTPUTDIR)\edlist.obj $(OUTPUTDIR)\symbtabl.obj $(OUTPUTDIR)\winstyle.obj

!include $(WXDIR)\src\makeprog.wat


