#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 16 January 2009                                                     *
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
	$(MMS)$(MMSQUALIFIERS) combo.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) combo_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
combo.exe : combo.obj
	cxxlink combo,[--.lib]vms/opt
.else
.ifdef __WXGTK__
combo_gtk.exe : combo.obj
	cxxlink/exec=combo_gtk.exe combo,[--.lib]vms_gtk/opt
.endif
.endif

combo.obj : combo.cpp
