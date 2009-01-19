#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 19 January 2009                                                     *
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
	$(MMS)$(MMSQUALIFIERS) image.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) image_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
image.exe : image.obj,canvas.obj
	cxxlink image,canvas,[--.lib]vms/opt
.else
.ifdef __WXGTK__
image_gtk.exe : image.obj,canvas.obj
	cxxlink/exec=image_gtk.exe image,canvas,[--.lib]vms_gtk/opt
.endif
.endif

image.obj : image.cpp
canvas.obj : canvas.cpp
