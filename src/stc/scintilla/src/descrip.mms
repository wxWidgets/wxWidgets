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
CXX_DEFINE = /name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/define=(__WX__=1,__WXMOTIF__=1,__USE_STD_IOSTREAM=1,NO_CXX11_REGEX=1,WXBUILDING=1)\
	/include=([],[-.src],[-.include],[-.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,NO_CXX11_REGEX=1,WXBUILDING=1)/name=(as_is,short)\
	/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,__USE_STD_IOSTREAM=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)\
	   /include=([],[-.src],[-.include],[-.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,__USE_STD_IOSTREAM=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)\
	   /include=([],[-.src],[-.include],[-.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee/name=(as_is,short)\
	/ieee=denorm/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__=1,__USE_STD_IOSTREAM=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/include=([],[-.src],[-.include],[-.lexlib])
CC_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__=1,NO_CXX11_REGEX=1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/include=([],[-.src],[-.include],[-.lexlib])
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

OBJECTS=AutoComplete.obj,CallTip.obj,CaseConvert.obj,CaseFolder.obj,\
	Catalogue.obj,CellBuffer.obj,CharClassify.obj,ContractionState.obj,\
	Decoration.obj,Document.obj,Editor.obj,ExternalLexer.obj,Indicator.obj,\
	KeyMap.obj,LineMarker.obj,PerLine.obj,PositionCache.obj,RESearch.obj,\
	RunStyles.obj,ScintillaBase.obj,Selection.obj,Style.obj,UniConversion.obj,\
	ViewStyle.obj,XPM.obj,EditModel.obj,EditView.obj,MarginView.obj

SOURCES=AutoComplete.cxx,CallTip.cxx,CaseConvert.cxx,CaseFolder.cxx,\
	Catalogue.cxx,CellBuffer.cxx,CharClassify.cxx,ContractionState.cxx,\
	Decoration.cxx,Document.cxx,Editor.cxx,ExternalLexer.cxx,Indicator.cxx,\
	KeyMap.cxx,LineMarker.cxx,PerLine.cxx,PositionCache.cxx,RESearch.cxx,\
	RunStyles.cxx,ScintillaBase.cxx,Selection.cxx,Style.cxx,UniConversion.cxx,\
	ViewStyle.cxx,XPM.cxx,EditModel.cxx,EditView.cxx,MarginView.cxx

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

AutoComplete.obj : AutoComplete.cxx
CallTip.obj : CallTip.cxx
CaseConvert.obj : CaseConvert.cxx
CaseFolder.obj : CaseFolder.cxx
Catalogue.obj : Catalogue.cxx
CellBuffer.obj : CellBuffer.cxx
CharClassify.obj : CharClassify.cxx
ContractionState.obj : ContractionState.cxx
Decoration.obj : Decoration.cxx
Document.obj : Document.cxx
Editor.obj : Editor.cxx
	cxx $(CXXFLAGS)$(CXX_DEFINE)/nowarn Editor.cxx
ExternalLexer.obj : ExternalLexer.cxx
	cxx $(CXXFLAGS)$(CXX_DEFINE)/warn=disable=(CONPTRLOSBIT)\
	ExternalLexer.cxx
Indicator.obj : Indicator.cxx
KeyMap.obj : KeyMap.cxx
LineMarker.obj : LineMarker.cxx
PerLine.obj : PerLine.cxx
PositionCache.obj : PositionCache.cxx
RESearch.obj : RESearch.cxx
RunStyles.obj : RunStyles.cxx
ScintillaBase.obj : ScintillaBase.cxx
	cxx $(CXXFLAGS)$(CXX_DEFINE)/warn=disable=(CONPTRLOSBIT)\
	ScintillaBase.cxx
Selection.obj : Selection.cxx
Style.obj : Style.cxx
UniConversion.obj : UniConversion.cxx
ViewStyle.obj : ViewStyle.cxx
XPM.obj : XPM.cxx
EditModel.obj : EditModel.cxx
EditView.obj : EditView.cxx
MarginView.obj : MarginView.cxx
