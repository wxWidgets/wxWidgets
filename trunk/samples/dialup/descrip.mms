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
	$(MMS)$(MMSQUALIFIERS) nettest.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) nettest_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
nettest.exe : nettest.obj
	cxxlink nettest,[--.lib]vms/opt
.else
.ifdef __WXGTK__
nettest_gtk.exe : nettest.obj
	cxxlink/exec=nettest_gtk.exe nettest,[--.lib]vms_gtk/opt
.endif
.endif

nettest.obj : nettest.cpp
