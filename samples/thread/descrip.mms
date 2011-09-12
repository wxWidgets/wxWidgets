#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 12 September 2011                                                   *
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
	$(MMS)$(MMSQUALIFIERS) thread.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) thread_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) thread_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) thread_x11.exe
.endif
.endif
.endif
.endif

.ifdef __WXMOTIF__
thread.exe : thread.obj
	cxxlink thread,[--.lib]vms/opt
.else
.ifdef __WXGTK__
thread_gtk.exe : thread.obj
	cxxlink/exec=thread_gtk.exe thread,[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
thread_gtk2.exe : thread.obj
	cxxlink/exec=thread_gtk2.exe thread,[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
thread_x11.exe : thread.obj
	cxxlink/exec=thread_x11.exe thread,[--.lib]vms_x11_univ/opt
.endif
.endif
.endif
.endif

thread.obj : thread.cpp
