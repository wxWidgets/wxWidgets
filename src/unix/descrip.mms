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
.c.obj :
	cc $(CFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		dialup.obj,\
		dir.obj,\
		fontenum.obj,\
		fontutil.obj,\
		gsocket.obj,\
		mimetype.obj,\
		threadpsx.obj,\
		utilsunx.obj

SOURCES = \
		dialup.cpp,\
		dir.cpp,\
		fontenum.cpp,\
		fontutil.cpp,\
		gsocket.c,\
		mimetype.cpp,\
		threadpsx.cpp,\
		utilsunx.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
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
