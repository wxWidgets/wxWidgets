#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = $(%WXWIN)

PROGRAM = dialoged
OBJECTS = dialoged.obj reseditr.obj dlghndlr.obj reswrite.obj winprop.obj edtree.obj edlist.obj symbtabl.obj winstyle.obj

!include $(WXDIR)\src\makeprog.wat


