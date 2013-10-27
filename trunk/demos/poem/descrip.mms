#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 5 October 2009                                                      *
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
	$(MMS)$(MMSQUALIFIERS) wxpoem.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) wxpoem_gtk.exe
.endif
.endif

OBJS=wxpoem.obj

.ifdef __WXMOTIF__
wxpoem.exe : $(OBJS) 
	cxxlink/exec=wxpoem.exe $(OBJS),[--.lib]vms/opt
.else
.ifdef __WXGTK__
wxpoem_gtk.exe : $(OBJS)
	cxxlink/exec=wxpoem_gtk.exe $(OBJS),[--.lib]vms_gtk/opt
.endif
.endif

wxpoem.obj : wxpoem.cpp
