#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 31 October 2001                                                     *
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
	$(MMS)$(MMSQUALIFIERS) dbtest.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) dbtest_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
dbtest.exe : dbtest.obj,listdb.obj
	cxxlink dbtest,listdb,[--.lib]vms/opt
.else
.ifdef __WXGTK__
dbtest_gtk.exe : dbtest.obj,listdb.obj
	cxxlink/exec=dbtest_gtk.exe dbtest,listdb,[--.lib]vms_gtk/opt
.endif
.endif

dbtest.obj : dbtest.cpp
listdb.obj : listdb.cpp
