#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 4 March 2020                                                        *
#                                                                            *
#*****************************************************************************

.first
	define wx [----.include.wx]

.suffixes : .cpp

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)/include=([],[-.src],[-.include])
CC_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)\
	/include=([],[-.src],[-.include])
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/include=([],[-.src],[-.include])
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	/include=([],[-.src],[-.include])
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/include=([],[-.src],[-.include])
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,WXBUILDING=1)/float=ieee/name=(as_is,short)\
	/ieee=denorm/include=([],[-.src],[-.include])
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/include=([],[-.src],[-.include])
CC_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/include=([],[-.src],[-.include])
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif
.endif

.cxx.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cxx
.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS=Accessor.obj,CharacterCategory.obj,CharacterSet.obj,LexerBase.obj,\
	LexerModule.obj,LexerNoExceptions.obj,LexerSimple.obj,PropSetSimple.obj,\
	StyleContext.obj,WordList.obj

SOURCES=Accessor.cxx,CharacterCategory.cxx,CharacterSet.cxx,LexerBase.cxx,\
	LexerModule.cxx,LexerNoExceptions.cxx,LexerSimple.cxx,PropSetSimple.cxx,\
	StyleContext.cxx,WordList.cxx

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [----.lib]libwx_motif.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK__
	library [----.lib]libwx_gtk.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK2__
	library [----.lib]libwx_gtk2.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXX11__
	library [----.lib]libwx_x11_univ.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
.endif
.endif
.endif
.endif

$(OBJECTS) : [----.include.wx]setup.h

Accessor.obj : Accessor.cxx
CharacterCategory.obj : CharacterCategory.cxx
CharacterSet.obj : CharacterSet.cxx
LexerBase.obj : LexerBase.cxx
LexerModule.obj : LexerModule.cxx
LexerNoExceptions.obj : LexerNoExceptions.cxx
LexerSimple.obj : LexerSimple.cxx
PropSetSimple.obj : PropSetSimple.cxx
StyleContext.obj : StyleContext.cxx
WordList.obj : WordList.cxx
