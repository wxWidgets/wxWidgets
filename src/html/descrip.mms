#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 4 October 2001                                                      *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
.ifdef __WXGTK__
.ifdef __WXUNIVERSAL__
CXX_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm
.else
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm
.endif
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
	helpctrl.obj,helpdata.obj,helpfrm.obj,htmlcell.obj,htmlfilt.obj,\
	htmlpars.obj,htmltag.obj,htmlwin.obj,htmprint.obj,m_dflist.obj,\
	m_fonts.obj,m_hline.obj,m_image.obj,m_layout.obj,m_links.obj,\
	m_list.obj,m_pre.obj,m_tables.obj,winpars.obj

SOURCES = \
	helpctrl.cpp,helpdata.cpp,helpfrm.cpp,htmlcell.cpp,htmlfilt.cpp,\
	htmlpars.cpp,htmltag.cpp,htmlwin.cpp,htmprint.cpp,m_dflist.cpp,\
	m_fonts.cpp,m_hline.cpp,m_image.cpp,m_layout.cpp,m_links.cpp,\
	m_list.cpp,m_pre.cpp,m_tables.cpp,winpars.cpp
  
all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXGTK__
.ifdef __WXUNIVERSAL__
	library [--.lib]libwx_gtk_univ.olb $(OBJECTS)
.else
	library [--.lib]libwx_gtk.olb $(OBJECTS)
.endif
.endif
.endif

helpctrl.obj : helpctrl.cpp
helpdata.obj : helpdata.cpp
helpfrm.obj : helpfrm.cpp
htmlcell.obj : htmlcell.cpp
htmlfilt.obj : htmlfilt.cpp
htmlpars.obj : htmlpars.cpp
htmltag.obj : htmltag.cpp
htmlwin.obj : htmlwin.cpp
htmprint.obj : htmprint.cpp
m_dflist.obj : m_dflist.cpp
m_fonts.obj : m_fonts.cpp
m_hline.obj : m_hline.cpp
m_image.obj : m_image.cpp
m_layout.obj : m_layout.cpp
m_links.obj : m_links.cpp
m_list.obj : m_list.cpp
m_pre.obj : m_pre.cpp
m_tables.obj : m_tables.cpp
winpars.obj : winpars.cpp
