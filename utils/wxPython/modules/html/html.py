# This file was created automatically by SWIG.
import htmlc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *
import wx,htmlhelper
widget = htmlc
class wxHtmlTagPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetName,(self,) + _args, _kwargs)
        return val
    def HasParam(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_HasParam,(self,) + _args, _kwargs)
        return val
    def GetParam(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetParam,(self,) + _args, _kwargs)
        return val
    def GetAllParams(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetAllParams,(self,) + _args, _kwargs)
        return val
    def IsEnding(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_IsEnding,(self,) + _args, _kwargs)
        return val
    def HasEnding(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_HasEnding,(self,) + _args, _kwargs)
        return val
    def GetBeginPos(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetBeginPos,(self,) + _args, _kwargs)
        return val
    def GetEndPos1(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetEndPos1,(self,) + _args, _kwargs)
        return val
    def GetEndPos2(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTag_GetEndPos2,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlTag instance at %s>" % (self.this,)
class wxHtmlTag(wxHtmlTagPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlParserPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetFS(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_SetFS,(self,) + _args, _kwargs)
        return val
    def GetFS(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_GetFS,(self,) + _args, _kwargs)
        return val
    def Parse(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_Parse,(self,) + _args, _kwargs)
        return val
    def InitParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_InitParser,(self,) + _args, _kwargs)
        return val
    def DoneParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_DoneParser,(self,) + _args, _kwargs)
        return val
    def DoParsing(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_DoParsing,(self,) + _args, _kwargs)
        return val
    def AddTagHandler(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_AddTagHandler,(self,) + _args, _kwargs)
        return val
    def GetSource(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_GetSource,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlParser instance at %s>" % (self.this,)
class wxHtmlParser(wxHtmlParserPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlWinParserPtr(wxHtmlParserPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetDC(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetDC,(self,) + _args, _kwargs)
        return val
    def GetDC(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetDC,(self,) + _args, _kwargs)
        if val: val = wxDCPtr(val) 
        return val
    def GetCharHeight(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetCharHeight,(self,) + _args, _kwargs)
        return val
    def GetCharWidth(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetCharWidth,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFonts,(self,) + _args, _kwargs)
        return val
    def GetContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetContainer,(self,) + _args, _kwargs)
        return val
    def OpenContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_OpenContainer,(self,) + _args, _kwargs)
        return val
    def CloseContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_CloseContainer,(self,) + _args, _kwargs)
        return val
    def GetFontSize(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetFontSize,(self,) + _args, _kwargs)
        return val
    def SetFontSize(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFontSize,(self,) + _args, _kwargs)
        return val
    def GetFontBold(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetFontBold,(self,) + _args, _kwargs)
        return val
    def SetFontBold(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFontBold,(self,) + _args, _kwargs)
        return val
    def GetFontItalic(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetFontItalic,(self,) + _args, _kwargs)
        return val
    def SetFontItalic(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFontItalic,(self,) + _args, _kwargs)
        return val
    def GetFontUnderlined(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetFontUnderlined,(self,) + _args, _kwargs)
        return val
    def SetFontUnderlined(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFontUnderlined,(self,) + _args, _kwargs)
        return val
    def GetFontFixed(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetFontFixed,(self,) + _args, _kwargs)
        return val
    def SetFontFixed(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetFontFixed,(self,) + _args, _kwargs)
        return val
    def GetAlign(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetAlign,(self,) + _args, _kwargs)
        return val
    def SetAlign(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetAlign,(self,) + _args, _kwargs)
        return val
    def GetLinkColor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetLinkColor,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetLinkColor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetLinkColor,(self,) + _args, _kwargs)
        return val
    def GetActualColor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetActualColor,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetActualColor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetActualColor,(self,) + _args, _kwargs)
        return val
    def GetLink(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_GetLink,(self,) + _args, _kwargs)
        return val
    def SetLink(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetLink,(self,) + _args, _kwargs)
        return val
    def CreateCurrentFont(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_CreateCurrentFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlWinParser instance at %s>" % (self.this,)
class wxHtmlWinParser(wxHtmlWinParserPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlWinParser,_args,_kwargs)
        self.thisown = 1




class wxHtmlTagHandlerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTagHandler__setSelf,(self,) + _args, _kwargs)
        return val
    def SetParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTagHandler_SetParser,(self,) + _args, _kwargs)
        return val
    def GetParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTagHandler_GetParser,(self,) + _args, _kwargs)
        if val: val = wxHtmlParserPtr(val) 
        return val
    def ParseInner(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlTagHandler_ParseInner,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlTagHandler instance at %s>" % (self.this,)
class wxHtmlTagHandler(wxHtmlTagHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlTagHandler,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class wxHtmlWinTagHandlerPtr(wxHtmlTagHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinTagHandler__setSelf,(self,) + _args, _kwargs)
        return val
    def SetParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinTagHandler_SetParser,(self,) + _args, _kwargs)
        return val
    def GetParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinTagHandler_GetParser,(self,) + _args, _kwargs)
        if val: val = wxHtmlWinParserPtr(val) 
        return val
    def ParseInner(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinTagHandler_ParseInner,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlWinTagHandler instance at %s>" % (self.this,)
class wxHtmlWinTagHandler(wxHtmlWinTagHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlWinTagHandler,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class HtmlHistoryItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPos(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetPos,(self,) + _args, _kwargs)
        return val
    def SetPos(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_SetPos,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetPage,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetAnchor,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C HtmlHistoryItem instance at %s>" % (self.this,)
class HtmlHistoryItem(HtmlHistoryItemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_HtmlHistoryItem,_args,_kwargs)
        self.thisown = 1




class wxHtmlWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow__setSelf,(self,) + _args, _kwargs)
        return val
    def SetPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetPage,(self,) + _args, _kwargs)
        return val
    def LoadPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_LoadPage,(self,) + _args, _kwargs)
        return val
    def GetOpenedPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetOpenedPage,(self,) + _args, _kwargs)
        return val
    def SetRelatedFrame(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetRelatedFrame,(self,) + _args, _kwargs)
        return val
    def GetRelatedFrame(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetRelatedFrame,(self,) + _args, _kwargs)
        if val: val = wxFramePtr(val) 
        return val
    def SetRelatedStatusBar(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetRelatedStatusBar,(self,) + _args, _kwargs)
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetFonts,(self,) + _args, _kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetTitle,(self,) + _args, _kwargs)
        return val
    def SetBorders(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetBorders,(self,) + _args, _kwargs)
        return val
    def ReadCustomization(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_ReadCustomization,(self,) + _args, _kwargs)
        return val
    def WriteCustomization(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_WriteCustomization,(self,) + _args, _kwargs)
        return val
    def HistoryBack(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryBack,(self,) + _args, _kwargs)
        return val
    def HistoryForward(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryForward,(self,) + _args, _kwargs)
        return val
    def HistoryClear(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryClear,(self,) + _args, _kwargs)
        return val
    def GetInternalRepresentation(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetInternalRepresentation,(self,) + _args, _kwargs)
        return val
    def base_OnLinkClicked(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_base_OnLinkClicked,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlWindow instance at %s>" % (self.this,)
class wxHtmlWindow(wxHtmlWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlWindow,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)
        wx._StdWindowCallbacks(self)
        wx._StdOnScrollCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------

wxHtmlWinParser_AddTagHandler = htmlc.wxHtmlWinParser_AddTagHandler

wxHtmlWindow_AddFilter = htmlc.wxHtmlWindow_AddFilter



#-------------- VARIABLE WRAPPERS ------------------

