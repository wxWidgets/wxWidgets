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

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) minimal.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) minimal_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
minimal.exe : minimal.obj
	cxxlink minimal,[--.lib]vms/opt
.else
.ifdef __WXGTK__
minimal_gtk.exe : minimal.obj
	ass link.lis sys$output
	cxxlink/exec=minimal_gtk.exe minimal,[--.lib]vms_gtk/opt
	deass sys$output
.endif
.endif

minimal.obj : minimal.cpp
