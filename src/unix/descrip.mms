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
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS =       baseunix.obj,\
		dialup.obj,\
		dir.obj,\
		dlunix.obj,\
		fontenum.obj,\
		fontutil.obj,\
		gsocket.obj,\
		mimetype.obj,\
		threadpsx.obj,\
		utilsunx.obj,\
		utilsx11.obj,\
		joystick.obj,\
		snglinst.obj,\
		sound.obj,\
		sound_sdl.obj,\
		stdpaths.obj,\
		taskbarx11.obj

SOURCES =       baseunix.cpp,\
		dialup.cpp,\
		dir.cpp,\
		dlunix.cpp,\
		fontenum.cpp,\
		fontutil.cpp,\
		gsocket.cpp,\
		mimetype.cpp,\
		threadpsx.cpp,\
		utilsunx.cpp,\
		utilsx11.cpp,\
		joystick.cpp,\
		snglinst.cpp,\
		sound.cpp,\
		sound_sdl.cpp,\
		stdpaths.cpp,\
		taskbarx11.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
.endif
.endif
.endif

baseunix.obj : baseunix.cpp
dialup.obj : dialup.cpp
dir.obj : dir.cpp
dlunix.obj : dlunix.cpp
fontenum.obj : fontenum.cpp
fontutil.obj : fontutil.cpp
gsocket.obj : gsocket.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/nowarn gsocket.cpp
mimetype.obj : mimetype.cpp
threadpsx.obj : threadpsx.cpp
utilsunx.obj : utilsunx.cpp
utilsx11.obj : utilsx11.cpp
joystick.obj : joystick.cpp
snglinst.obj : snglinst.cpp
sound.obj : sound.cpp
sound_sdl.obj : sound_sdl.cpp
stdpaths.obj : stdpaths.cpp
taskbarx11.obj : taskbarx11.cpp
