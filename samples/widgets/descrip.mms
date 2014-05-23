#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 9 November 2011                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
.else
CXX_DEFINE =
.endif
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) widgets.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) widgets_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) widgets_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) widgets_x11.exe
.endif
.endif
.endif
.endif

OBJS=bmpcombobox.obj,button.obj,checkbox.obj,choice.obj,clrpicker.obj,\
	combobox.obj,datepick.obj,dirctrl.obj,dirpicker.obj,editlbox.obj,\
	filectrl.obj,filepicker.obj,fontpicker.obj,gauge.obj,hyperlnk.obj,\
	itemcontainer.obj,listbox.obj,notebook.obj,odcombobox.obj,\
	radiobox.obj,searchctrl.obj,slider.obj,spinbtn.obj,statbmp.obj,\
	static.obj,textctrl.obj,toggle.obj,widgets.obj

.ifdef __WXMOTIF__
widgets.exe : $(OBJS)
	cxxlink/exec=widgets.exe $(OBJS),[--.lib]vms/opt
.else
.ifdef __WXGTK__
widgets_gtk.exe : $(OBJS)
	cxxlink/exec=widgets_gtk.exe $(OBJS),[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
widgets_gtk2.exe : $(OBJS)
	cxxlink/exec=widgets_gtk2.exe $(OBJS),[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
widgets_x11.exe : $(OBJS)
	cxxlink/exec=widgets_x11.exe $(OBJS),[--.lib]vms_x11_univ/opt
.endif
.endif
.endif
.endif

bmpcombobox.obj : bmpcombobox.cpp
button.obj : button.cpp
checkbox.obj : checkbox.cpp
choice.obj : choice.cpp
clrpicker.obj : clrpicker.cpp
combobox.obj : combobox.cpp
datepick.obj : datepick.cpp
dirctrl.obj : dirctrl.cpp
dirpicker.obj : dirpicker.cpp
editlbox.obj : editlbox.cpp
filectrl.obj : filectrl.cpp
filepicker.obj : filepicker.cpp
fontpicker.obj : fontpicker.cpp
gauge.obj : gauge.cpp
hyperlnk.obj : hyperlnk.cpp
itemcontainer.obj : itemcontainer.cpp
listbox.obj : listbox.cpp
notebook.obj : notebook.cpp
odcombobox.obj : odcombobox.cpp
radiobox.obj : radiobox.cpp
searchctrl.obj : searchctrl.cpp
slider.obj : slider.cpp
spinbtn.obj : spinbtn.cpp
statbmp.obj : statbmp.cpp
static.obj : static.cpp
textctrl.obj : textctrl.cpp
toggle.obj : toggle.cpp
widgets.obj : widgets.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/object=widgets.obj widgets.cpp
