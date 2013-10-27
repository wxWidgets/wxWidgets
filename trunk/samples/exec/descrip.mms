#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                              *
# Date : 2 November 2010                                                     *
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
	$(MMS)$(MMSQUALIFIERS) exec.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) exec_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
exec.exe : exec.obj
	cxxlink exec,[--.lib]vms/opt
.else
.ifdef __WXGTK__
exec_gtk.exe : exec.obj
	cxxlink/exec=exec_gtk.exe exec,[--.lib]vms_gtk/opt
.endif
.endif

exec.obj : exec.cpp
