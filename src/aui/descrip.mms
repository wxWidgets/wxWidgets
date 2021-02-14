#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 4 March 2020                                                        *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /nowarn/define=(__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /nowarn/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /nowarn/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,VMS_GTK2,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXX11__
CXX_DEFINE = /nowarn/define=(__WXX11__=1,__WXUNIVERSAL__==1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = dockart.obj,floatpane.obj,framemanager.obj,auibook.obj,tabmdi.obj,\
	auibar.obj,tabart.obj

SOURCES = dockart.cpp floatpane.cpp framemanager.cpp auibook.cpp tabmdi.cpp \
	auibar.cpp tabart.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
.else
.ifdef __WXGTK2__
	library [--.lib]libwx_gtk2.olb $(OBJECTS)
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
.endif
.endif
.endif
.endif

$(OBJECTS) : [--.include.wx]setup.h

dockart.obj : dockart.cpp
floatpane.obj : floatpane.cpp
framemanager.obj : framemanager.cpp
auibook.obj :  auibook.cpp
tabmdi.obj : tabmdi.cpp
auibar.obj : auibar.cpp
tabart.obj : tabart.cpp
