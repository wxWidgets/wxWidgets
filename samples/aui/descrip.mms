#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 15 June 2006                                                        *
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
	$(MMS)$(MMSQUALIFIERS) auidemo.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) auidemo_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
auidemo.exe : auidemo.obj
	cxxlink auidemo,[--.lib]vms/opt
.else
.ifdef __WXGTK__
auidemo_gtk.exe : auidemo.obj
	cxxlink/exec=auidemo_gtk.exe auidemo,[--.lib]vms_gtk/opt
.endif
.endif

auidemo.obj : auidemo.cpp
