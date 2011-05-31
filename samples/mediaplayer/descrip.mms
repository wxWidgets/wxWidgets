#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 5 January 2011                                                      *
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
	$(MMS)$(MMSQUALIFIERS) mediaplayer.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) mediaplayer_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) mediaplayer_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) mediaplayer_x11.exe
.endif
.endif
.endif
.endif

.ifdef __WXMOTIF__
mediaplayer.exe : mediaplayer.obj
	cxxlink mediaplayer,[--.lib]vms/opt
.else
.ifdef __WXGTK__
mediaplayer_gtk.exe : mediaplayer.obj
	cxxlink/exec=mediaplayer_gtk.exe mediaplayer,[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
mediaplayer_gtk2.exe : mediaplayer.obj
	cxxlink/exec=mediaplayer_gtk2.exe mediaplayer,[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
mediaplayer_x11.exe : mediaplayer.obj
	cxxlink/exec=mediaplayer_x11.exe mediaplayer,[--.lib]vms_x11_univ/opt
.endif
.endif
.endif
.endif

mediaplayer.obj : mediaplayer.cpp
