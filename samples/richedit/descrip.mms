#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 15 October 2001                                                     *
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
	$(MMS)$(MMSQUALIFIERS) wxlayout.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) wxlayout_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
wxlayout.exe : wxlayout.obj,kbList.obj,wxllist.obj,wxlparser.obj,wxlwindow.obj

	cxxlink wxlayout,kbList.obj,wxllist.obj,wxlparser.obj,wxlwindow.obj,\
	[--.lib]vms/opt
.else
.ifdef __WXGTK__
wxlayout_gtk.exe : wxlayout.obj,kbList.obj,wxllist.obj,wxlparser.obj,\
	wxlwindow.obj
	cxxlink/exec=wxlayout_gtk.exe wxlayout,kbList.obj,wxllist.obj,\
	wxlparser.obj,wxlwindow.obj,[--.lib]vms_gtk/opt
.endif
.endif

wxlayout.obj : wxlayout.cpp
kbList.obj : kblist.cpp
wxllist.obj : wxllist.cpp
wxlparser.obj : wxlparser.cpp
wxlwindow.obj : wxlwindow.cpp
