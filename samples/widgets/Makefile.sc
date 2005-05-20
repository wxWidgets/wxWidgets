#Makefile from wxHatch for Symantec/Digital Mars compiler 
WXDIR=..\..
TARGET=widgets
OBJECTS = $(TARGET).obj button.obj combobox.obj gauge.obj listbox.obj\
 notebook.obj radiobox.obj slider.obj spinbtn.obj static.obj textctrl.obj
EXTRALIBS =
include $(WXDIR)\src\makeprog.sc
