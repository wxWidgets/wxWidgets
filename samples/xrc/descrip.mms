#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 15 November 2010                                                    *
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
	$(MMS)$(MMSQUALIFIERS) xrcdemo.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) xrcdemo_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) xrcdemo_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) xrcdemo_x11.exe
.endif
.endif
.endif
.endif

.ifdef __WXMOTIF__
xrcdemo.exe : xrcdemo.obj,custclas.obj,derivdlg.obj,myframe.obj,objrefdlg.obj
	cxxlink xrcdemo,custclas.obj,derivdlg.obj,myframe.obj,objrefdlg.obj,\
	[--.lib]vms/opt
.else
.ifdef __WXGTK__
xrcdemo_gtk.exe : xrcdemo.obj,custclas.obj,derivdlg.obj,myframe.obj,\
	objrefdlg.obj
	cxxlink/exec=xrcdemo_gtk.exe xrcdemo,custclas.obj,derivdlg.obj,\
	myframe.obj,objrefdlg.obj,[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
xrcdemo_gtk2.exe : xrcdemo.obj,custclas.obj,derivdlg.obj,myframe.obj
	cxxlink/exec=xrcdemo_gtk2.exe xrcdemo,custclas.obj,derivdlg.obj,\
	myframe.obj,[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
xrcdemo_x11.exe : xrcdemo.obj,custclas.obj,derivdlg.obj,myframe.obj
	cxxlink/exec=xrcdemo_x11.exe xrcdemo,custclas.obj,derivdlg.obj,\
	myframe.obj,[--.lib]vms_x11_univ/opt
.endif
.endif
.endif
.endif

xrcdemo.obj : xrcdemo.cpp
custclas.obj : custclas.cpp
derivdlg.obj : derivdlg.cpp
myframe.obj : myframe.cpp
objrefdlg.obj : objrefdlg.cpp
