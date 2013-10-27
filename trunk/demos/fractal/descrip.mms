#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 3 October 2009                                                      *
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
	$(MMS)$(MMSQUALIFIERS) fractal.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) fractal_gtk.exe
.endif
.endif

OBJS=fractal.obj

.ifdef __WXMOTIF__
fractal.exe : $(OBJS) 
	cxxlink/exec=fractal.exe $(OBJS),[--.lib]vms/opt
.else
.ifdef __WXGTK__
fractal_gtk.exe : $(OBJS)
	cxxlink/exec=fractal_gtk.exe $(OBJS),[--.lib]vms_gtk/opt
.endif
.endif

fractal.obj : fractal.cpp
