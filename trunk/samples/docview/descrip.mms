#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 2 November 2001                                                     *
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
CXX_DEFINE =
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) docview.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) docview_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
docview.exe : docview.obj,doc.obj,view.obj
	cxxlink docview,doc,view,[--.lib]vms/opt
.else
.ifdef __WXGTK__
docview_gtk.exe : docview.obj,doc.obj,view.obj
	cxxlink/exec=docview_gtk.exe docview,doc,view,[--.lib]vms_gtk/opt
.endif
.endif

docview.obj : docview.cpp
view.obj : view.cpp
doc.obj : doc.cpp
