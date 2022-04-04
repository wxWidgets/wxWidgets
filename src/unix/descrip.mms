#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 23 September 2021                                                   *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,VMS_GTK2,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1,WXBUILDING=1)/float=ieee\
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

OBJECTS = appunix.obj,apptraits.obj,\
		dialup.obj,\
		dir.obj,\
		dlunix.obj,\
		fontenum.obj,\
		fontutil.obj,\
		sockunix.obj,\
		mimetype.obj,\
		threadpsx.obj,\
		utilsunx.obj,\
		utilsx11.obj,\
		joystick.obj,\
		snglinst.obj,\
		sound.obj,\
		sound_sdl.obj,\
		stdpaths.obj,\
		taskbarx11.obj,\
		timerunx.obj,evtloopunix.obj,fdiounix.obj,uiactionx11.obj,\
		mediactrl.obj,wakeuppipe.obj,unix_uilocale.obj

OBJECTS2=displayx11.obj


SOURCES = appunix.cpp,apptraits.cpp,\
		dialup.cpp,\
		dir.cpp,\
		displayx11.cpp,\
		dlunix.cpp,\
		fontenum.cpp,\
		fontutil.cpp,\
		sockunix.cpp,\
		mimetype.cpp,\
		threadpsx.cpp,\
		utilsunx.cpp,\
		utilsx11.cpp,\
		joystick.cpp,\
		snglinst.cpp,\
		sound.cpp,\
		sound_sdl.cpp,\
		stdpaths.cpp,\
		taskbarx11.cpp,\
		timerunx.cpp,evtloopunix.cpp,fdiounix.cpp,uiactionx11.cpp,\
		mediactrl.cpp,wakeuppipe.cpp,uilocale.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS2)
	library [--.lib]libwx_motif.olb $(OBJECTS2)
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS2)
	library [--.lib]libwx_gtk.olb $(OBJECTS2)
.else
.ifdef __WXGTK2__
	library [--.lib]libwx_gtk2.olb $(OBJECTS)
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS2)
	library [--.lib]libwx_x11_univ.olb $(OBJECTS2)
.endif
.endif
.endif
.endif

$(OBJECTS) : [--.include.wx]setup.h
$(OBJECTS2) : [--.include.wx]setup.h

appunix.obj : appunix.cpp
apptraits.obj : apptraits.cpp
dialup.obj : dialup.cpp
dir.obj : dir.cpp
dlunix.obj : dlunix.cpp
fontenum.obj : fontenum.cpp
fontutil.obj : fontutil.cpp
sockunix.obj : sockunix.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/nowarn sockunix.cpp
mimetype.obj : mimetype.cpp
threadpsx.obj : threadpsx.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/nowarn threadpsx.cpp
utilsunx.obj : utilsunx.cpp
utilsx11.obj : utilsx11.cpp
joystick.obj : joystick.cpp
snglinst.obj : snglinst.cpp
sound.obj : sound.cpp
sound_sdl.obj : sound_sdl.cpp
stdpaths.obj : stdpaths.cpp
taskbarx11.obj : taskbarx11.cpp
displayx11.obj : displayx11.cpp
	pipe gsed -e "s/X11\/extensions/X11/" < $(MMS$TARGET_NAME).cpp\
	> $(MMS$TARGET_NAME).cpp_
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp_
	delete $(MMS$TARGET_NAME).cpp_;*
timerunx.obj : timerunx.cpp
evtloopunix.obj : evtloopunix.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/nowarn evtloopunix.cpp
fdiounix.obj : fdiounix.cpp
uiactionx11.obj : uiactionx11.cpp
mediactrl.obj : mediactrl.cpp
wakeuppipe.obj : wakeuppipe.cpp
unix_uilocale.obj : uilocale.cpp
	cop uilocale.cpp unix_uilocale.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE) unix_uilocale.cpp
	delete unix_uilocale.cpp;*
