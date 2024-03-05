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
CXX_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/name=(as_is,short)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	/assume=(nostdnew,noglobal_array_new)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
.else
.ifdef __WXGTK2__
CXX_DEFINE=/define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	/assume=(nostdnew,noglobal_array_new)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
CC_DEFINE=/define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
.else
.ifdef __WXX11__
CXX_DEFINE=\
	/define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
CC_DEFINE=\
	/define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,SCI_LEXER=1,LINK_LEXERS=1,WXBUILDING=1)\
	/float=ieee/name=(as_is,short)\
	/include=([],[.scintilla.src],[.scintilla.include],[.scintilla.lexlib])
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

OBJECTS=PlatWX.obj,ScintillaWX.obj,stc.obj

SOURCES=PlatWX.cpp,ScintillaWX.cpp,stc.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK2__
	library [--.lib]libwx_gtk2.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
.endif
.endif
.endif
.endif
	set def [.scintilla.src]
	$(MMS)$(MMSQUALIFIERS)
	set def [-.lexlib]
	$(MMS)$(MMSQUALIFIERS)
	set def [-.lexers]
	$(MMS)$(MMSQUALIFIERS)
	set def [--]

$(OBJECTS) : [--.include.wx]setup.h

PlatWX.obj : PlatWX.cpp
ScintillaWX.obj : ScintillaWX.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/warn=disable=(CONPTRLOSBIT)\
	ScintillaWX.cpp
stc.obj : stc.cpp
	cxx $(CXXFLAGS)$(CXX_DEFINE)/warn=disable=(CONPTRLOSBIT) stc.cpp
