#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)
.else
CXX_DEFINE =
.endif
.endif

CXXFLAGS=/list/show=all

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) drawing.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) drawing_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
drawing.exe : drawing.obj
	cxxlink drawing,[--.lib]vms/opt
.else
.ifdef __WXGTK__
drawing_gtk.exe : drawing.obj
	cxxlink/exec=drawing_gtk.exe drawing,[--.lib]vms_gtk/opt
.endif
.endif

drawing.obj : drawing.cpp
