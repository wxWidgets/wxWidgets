#
# Makefile for WATCOM
#
#

WXDIR = $(%WXWIN)

PROGRAM = wxrcedit
EXTRALIBS = $(WXDIR)\lib\wxxml.lib
OBJECTS=edapp.obj editor.obj nodehnd.obj xmlhelpr.obj preview.obj nodesdb.obj pe_basic.obj pe_adv.obj propedit.obj propframe.obj splittree.obj

!include $(WXDIR)\src\makeprog.wat

