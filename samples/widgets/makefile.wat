#
# Makefile for WATCOM
#
# Created by Julian Smart, January 1999
# 
#

WXDIR = $(%WXWIN)

PROGRAM = widgets
OBJECTS = $(PROGRAM).obj button.obj combobox.obj gauge.obj listbox.obj notebook.obj radiobox.obj slider.obj spinbtn.obj static.obj textctrl.obj

!include $(WXDIR)\src\makeprog.wat


