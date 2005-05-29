#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 4 October 2000                                                      *
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
	$(MMS)$(MMSQUALIFIERS) shaped.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) shaped_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
shaped.exe : shaped.obj
	cxxlink shaped,[--.lib]vms/opt
.else
.ifdef __WXGTK__
shaped_gtk.exe : shaped.obj
	cxxlink/exec=shaped_gtk.exe shaped,[--.lib]vms_gtk/opt
.endif
.endif

shaped.obj : shaped.cpp
