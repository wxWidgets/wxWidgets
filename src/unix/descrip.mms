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
		dialup.obj,\
		dir.obj,\
		fontenum.obj,\
		fontutil.obj,\
		gsocket.obj,\
		mimetype.obj,\
		threadpsx.obj,\
		utilsunx.obj,\
		utilsx11.obj

SOURCES = \
		dialup.cpp,\
		dir.cpp,\
		fontenum.cpp,\
		fontutil.cpp,\
		gsocket.c,\
		mimetype.cpp,\
		threadpsx.cpp,\
		utilsunx.cpp,\
		utilsx11.cpp

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

dialup.obj : dialup.cpp
dir.obj : dir.cpp
fontenum.obj : fontenum.cpp
fontutil.obj : fontutil.cpp
gsocket.obj : gsocket.c
mimetype.obj : mimetype.cpp
threadpsx.obj : threadpsx.cpp
utilsunx.obj : utilsunx.cpp
utilsx11.obj : utilsx11.cpp
