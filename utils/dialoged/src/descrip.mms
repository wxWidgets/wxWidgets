#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [---.include.wx]

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
	$(MMS)$(MMSQUALIFIERS) dialoged.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) dialoged_gtk.exe
.endif
.endif

OBJECTS=dialoged.obj,reseditr.obj,dlghndlr.obj,reswrite.obj,winprop.obj,\
	edtree.obj,edlist.obj,symbtabl.obj,winstyle.obj

.ifdef __WXMOTIF__
dialoged.exe : $(OBJECTS)
	cxxlink $(OBJECTS),[---.lib]vms/opt
.else
.ifdef __WXGTK__
dialoged_gtk.exe : $(OBJECTS)
	cxxlink $(OBJECTS),[---.lib]vms_gtk/opt
.endif
.endif

dialoged.obj : dialoged.cpp
reseditr.obj : reseditr.cpp
dlghndlr.obj : dlghndlr.cpp
reswrite.obj : reswrite.cpp
winprop.obj : winprop.cpp
edtree.obj : edtree.cpp
edlist.obj : edlist.cpp
symbtabl.obj : symbtabl.cpp
winstyle.obj : winstyle.cpp
