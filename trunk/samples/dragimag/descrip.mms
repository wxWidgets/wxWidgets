#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 9 October 2009                                                      *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)/incl=([],[-])
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/incl=([],[-])
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/incl=([],[-])
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)/incl=([],[-])
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
	$(MMS)$(MMSQUALIFIERS) dragimag.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) dragimag_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) dragimag_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) dragimag_x11.exe
.endif
.endif
.endif
.endif

OBJS=dragimag.obj

.ifdef __WXMOTIF__
dragimag.exe : $(OBJS)
	cxxlink $(OBJS),[--.lib]vms/opt
.else
.ifdef __WXGTK__
dragimag_gtk.exe : $(OBJS)
	cxxlink/exec=dragimag_gtk.exe $(OBJS),[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
dragimag_gtk2.exe : $(OBJS)
	cxxlink/exec=dragimag_gtk2.exe $(OBJS),[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
dragimag_x11.exe : $(OBJS)
	cxxlink/exec=dragimag_x11.exe $(OBJS),[--.lib]vms_x11_univ/opt
.endif
.endif
.endif
.endif

dragimag.obj : dragimag.cpp
