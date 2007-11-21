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
	$(MMS)$(MMSQUALIFIERS) game.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) game_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
game.exe : game.obj bombs1.obj bombs.obj
	cxxlink game,bombs1,bombs,[--.lib]vms/opt
.else
.ifdef __WXGTK__
game_gtk.exe : game.obj bombs1.obj bombs.obj
	cxxlink/exec=game_gtk.exe game,bombs1,bombs,[--.lib]vms_gtk/opt
.endif
.endif

game.obj : game.cpp
bombs1.obj : bombs1.cpp
bombs.obj : bombs.cpp
