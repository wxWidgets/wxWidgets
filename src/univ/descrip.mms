#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 14 March 2002                                                       *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

CXX_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		bmpbuttn.obj,\
		button.obj,\
		checkbox.obj,\
		checklst.obj,\
		choice.obj,\
		colschem.obj,\
		combobox.obj,\
		control.obj,\
		dialog.obj,\
		framuniv.obj,\
		gauge.obj,\
		gtk.obj,\
		inpcons.obj,\
		inphand.obj,\
		listbox.obj,\
		menu.obj,\
		notebook.obj,\
		radiobox.obj,\
		radiobut.obj,\
		renderer.obj,\
		scrarrow.obj,\
		scrolbar.obj,\
		scrthumb.obj,\
		slider.obj,\
		spinbutt.obj,\
		statbmp.obj,\
		statbox.obj,\
		statline.obj,\
		stattext.obj,\
		statusbr.obj,\
		textctrl.obj,\
		theme.obj,\
		toolbar.obj,\
		topluniv.obj,\
		winuniv.obj

SOURCES =\
		bmpbuttn.cpp \
		button.cpp \
		checkbox.cpp \
		checklst.cpp \
		choice.cpp \
		colschem.cpp \
		combobox.cpp \
		control.cpp \
		dialog.cpp \
		framuniv.cpp \
		gauge.cpp \
		[.themes]gtk.cpp \
		inpcons.cpp \
		inphand.cpp \
		listbox.cpp \
		menu.cpp \
		notebook.cpp \
		radiobox.cpp \
		radiobut.cpp \
		renderer.cpp \
		scrarrow.cpp \
		scrolbar.cpp \
		scrthumb.cpp \
		slider.cpp \
		spinbutt.cpp \
		statbmp.cpp \
		statbox.cpp \
		statline.cpp \
		stattext.cpp \
		statusbr.cpp \
		textctrl.cpp \
		theme.cpp \
		toolbar.cpp \
		topluniv.cpp \
		winuniv.cpp
   
all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
	library [--.lib]libwx_gtk_univ.olb $(OBJECTS)

bmpbuttn.obj : bmpbuttn.cpp
button.obj : button.cpp
checkbox.obj : checkbox.cpp
checklst.obj : checklst.cpp
choice.obj : choice.cpp
colschem.obj : colschem.cpp
combobox.obj : combobox.cpp
control.obj : control.cpp
dialog.obj : dialog.cpp
framuniv.obj : framuniv.cpp
gauge.obj : gauge.cpp
gtk.obj : [.themes]gtk.cpp
	cxx/obj=[]gtk.obj$(CXXFLAGS)$(CXX_DEFINE) [.themes]gtk.cpp
inpcons.obj : inpcons.cpp
inphand.obj : inphand.cpp
listbox.obj : listbox.cpp
menu.obj : menu.cpp
notebook.obj : notebook.cpp
radiobox.obj : radiobox.cpp
radiobut.obj : radiobut.cpp
renderer.obj : renderer.cpp
scrarrow.obj : scrarrow.cpp
scrolbar.obj : scrolbar.cpp
scrthumb.obj : scrthumb.cpp
slider.obj : slider.cpp
spinbutt.obj : spinbutt.cpp
statbmp.obj : statbmp.cpp
statbox.obj : statbox.cpp
statline.obj : statline.cpp
stattext.obj : stattext.cpp
statusbr.obj : statusbr.cpp
textctrl.obj : textctrl.cpp
theme.obj : theme.cpp
toolbar.obj : toolbar.cpp
topluniv.obj : topluniv.cpp
winuniv.obj : winuniv.cpp
