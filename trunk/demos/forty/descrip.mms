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
	$(MMS)$(MMSQUALIFIERS) forty.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) forty_gtk.exe
.endif
.endif

OBJS=canvas.obj,card.obj,forty.obj,game.obj,pile.obj,playerdg.obj,scoredg.obj,\
	scorefil.obj

.ifdef __WXMOTIF__
forty.exe : $(OBJS) 
	cxxlink/exec=forty.exe $(OBJS),[--.lib]vms/opt
.else
.ifdef __WXGTK__
forty_gtk.exe : $(OBJS)
	cxxlink/exec=forty_gtk.exe $(OBJS),[--.lib]vms_gtk/opt
.endif
.endif

canvas.obj : canvas.cpp
card.obj : card.cpp
forty.obj : forty.cpp
game.obj : game.cpp
pile.obj : pile.cpp
playerdg.obj : playerdg.cpp
scoredg.obj : scoredg.cpp
scorefil.obj : scorefil.cpp
