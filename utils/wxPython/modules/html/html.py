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

from sizers import *
import wx
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
    def PushTagHandler(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_PushTagHandler,(self,) + _args, _kwargs)
        return val
    def PopTagHandler(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlParser_PopTagHandler,(self,) + _args, _kwargs)
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
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def OpenContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_OpenContainer,(self,) + _args, _kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def SetContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_SetContainer,(self,) + _args, _kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def CloseContainer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWinParser_CloseContainer,(self,) + _args, _kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
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




class wxHtmlCellPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetParent(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_SetParent,(self,) + _args, _kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetParent,(self,) + _args, _kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def GetPosX(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetPosX,(self,) + _args, _kwargs)
        return val
    def GetPosY(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetPosY,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetDescent(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetDescent,(self,) + _args, _kwargs)
        return val
    def GetLink(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetLink,(self,) + _args, _kwargs)
        return val
    def GetNext(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_GetNext,(self,) + _args, _kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def SetPos(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_SetPos,(self,) + _args, _kwargs)
        return val
    def SetLink(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_SetLink,(self,) + _args, _kwargs)
        return val
    def SetNext(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_SetNext,(self,) + _args, _kwargs)
        return val
    def Layout(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_Layout,(self,) + _args, _kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_Draw,(self,) + _args, _kwargs)
        return val
    def DrawInvisible(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_DrawInvisible,(self,) + _args, _kwargs)
        return val
    def Find(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_Find,(self,) + _args, _kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def AdjustPagebreak(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_AdjustPagebreak,(self,) + _args, _kwargs)
        return val
    def SetCanLiveOnPagebreak(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlCell_SetCanLiveOnPagebreak,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlCell instance at %s>" % (self.this,)
class wxHtmlCell(wxHtmlCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlCell,_args,_kwargs)
        self.thisown = 1




class wxHtmlContainerCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def InsertCell(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_InsertCell,(self,) + _args, _kwargs)
        return val
    def SetAlignHor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetAlignHor,(self,) + _args, _kwargs)
        return val
    def GetAlignHor(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetAlignHor,(self,) + _args, _kwargs)
        return val
    def SetAlignVer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetAlignVer,(self,) + _args, _kwargs)
        return val
    def GetAlignVer(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetAlignVer,(self,) + _args, _kwargs)
        return val
    def SetIndent(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetIndent,(self,) + _args, _kwargs)
        return val
    def GetIndent(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetIndent,(self,) + _args, _kwargs)
        return val
    def GetIndentUnits(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetIndentUnits,(self,) + _args, _kwargs)
        return val
    def SetAlign(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetAlign,(self,) + _args, _kwargs)
        return val
    def SetWidthFloat(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetWidthFloat,(self,) + _args, _kwargs)
        return val
    def SetWidthFloatFromTag(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetWidthFloatFromTag,(self,) + _args, _kwargs)
        return val
    def SetMinHeight(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetMinHeight,(self,) + _args, _kwargs)
        return val
    def GetMaxLineWidth(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetMaxLineWidth,(self,) + _args, _kwargs)
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetBackgroundColour,(self,) + _args, _kwargs)
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_SetBorder,(self,) + _args, _kwargs)
        return val
    def GetFirstCell(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlContainerCell_GetFirstCell,(self,) + _args, _kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlContainerCell instance at %s>" % (self.this,)
class wxHtmlContainerCell(wxHtmlContainerCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlContainerCell,_args,_kwargs)
        self.thisown = 1




class wxHtmlWidgetCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxHtmlWidgetCell instance at %s>" % (self.this,)
class wxHtmlWidgetCell(wxHtmlWidgetCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlWidgetCell,_args,_kwargs)
        self.thisown = 1




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
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def GetParser(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetParser,(self,) + _args, _kwargs)
        if val: val = wxHtmlWinParserPtr(val) 
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




class wxHtmlDCRendererPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlc=htmlc):
        if self.thisown == 1 :
            htmlc.delete_wxHtmlDCRenderer(self)
    def SetDC(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlDCRenderer_SetDC,(self,) + _args, _kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlDCRenderer_SetSize,(self,) + _args, _kwargs)
        return val
    def SetHtmlText(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlDCRenderer_SetHtmlText,(self,) + _args, _kwargs)
        return val
    def Render(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlDCRenderer_Render,(self,) + _args, _kwargs)
        return val
    def GetTotalHeight(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlDCRenderer_GetTotalHeight,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlDCRenderer instance at %s>" % (self.this,)
class wxHtmlDCRenderer(wxHtmlDCRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlDCRenderer,_args,_kwargs)
        self.thisown = 1




class wxHtmlPrintoutPtr(wxPrintoutPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlc=htmlc):
        if self.thisown == 1 :
            htmlc.delete_wxHtmlPrintout(self)
    def SetHtmlText(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlPrintout_SetHtmlText,(self,) + _args, _kwargs)
        return val
    def SetHtmlFile(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlPrintout_SetHtmlFile,(self,) + _args, _kwargs)
        return val
    def SetHeader(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlPrintout_SetHeader,(self,) + _args, _kwargs)
        return val
    def SetFooter(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlPrintout_SetFooter,(self,) + _args, _kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlPrintout_SetMargins,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlPrintout instance at %s>" % (self.this,)
class wxHtmlPrintout(wxHtmlPrintoutPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlPrintout,_args,_kwargs)
        self.thisown = 1




class wxHtmlEasyPrintingPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlc=htmlc):
        if self.thisown == 1 :
            htmlc.delete_wxHtmlEasyPrinting(self)
    def PreviewFile(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PreviewFile,(self,) + _args, _kwargs)
        return val
    def PreviewText(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PreviewText,(self,) + _args, _kwargs)
        return val
    def PrintFile(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PrintFile,(self,) + _args, _kwargs)
        return val
    def PrintText(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PrintText,(self,) + _args, _kwargs)
        return val
    def PrinterSetup(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PrinterSetup,(self,) + _args, _kwargs)
        return val
    def PageSetup(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_PageSetup,(self,) + _args, _kwargs)
        return val
    def SetHeader(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_SetHeader,(self,) + _args, _kwargs)
        return val
    def SetFooter(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_SetFooter,(self,) + _args, _kwargs)
        return val
    def GetPrintData(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_GetPrintData,(self,) + _args, _kwargs)
        if val: val = wxPrintDataPtr(val) 
        return val
    def GetPageSetupData(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlEasyPrinting_GetPageSetupData,(self,) + _args, _kwargs)
        if val: val = wxPageSetupDialogDataPtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlEasyPrinting instance at %s>" % (self.this,)
class wxHtmlEasyPrinting(wxHtmlEasyPrintingPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlEasyPrinting,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

wxHtmlWinParser_AddTagHandler = htmlc.wxHtmlWinParser_AddTagHandler

wxHtmlWindow_AddFilter = htmlc.wxHtmlWindow_AddFilter



#-------------- VARIABLE WRAPPERS ------------------

wxHTML_ALIGN_LEFT = htmlc.wxHTML_ALIGN_LEFT
wxHTML_ALIGN_CENTER = htmlc.wxHTML_ALIGN_CENTER
wxHTML_ALIGN_RIGHT = htmlc.wxHTML_ALIGN_RIGHT
wxHTML_ALIGN_BOTTOM = htmlc.wxHTML_ALIGN_BOTTOM
wxHTML_ALIGN_TOP = htmlc.wxHTML_ALIGN_TOP
wxHTML_CLR_FOREGROUND = htmlc.wxHTML_CLR_FOREGROUND
wxHTML_CLR_BACKGROUND = htmlc.wxHTML_CLR_BACKGROUND
wxHTML_UNITS_PIXELS = htmlc.wxHTML_UNITS_PIXELS
wxHTML_UNITS_PERCENT = htmlc.wxHTML_UNITS_PERCENT
wxHTML_INDENT_LEFT = htmlc.wxHTML_INDENT_LEFT
wxHTML_INDENT_RIGHT = htmlc.wxHTML_INDENT_RIGHT
wxHTML_INDENT_TOP = htmlc.wxHTML_INDENT_TOP
wxHTML_INDENT_BOTTOM = htmlc.wxHTML_INDENT_BOTTOM
wxHTML_INDENT_HORIZONTAL = htmlc.wxHTML_INDENT_HORIZONTAL
wxHTML_INDENT_VERTICAL = htmlc.wxHTML_INDENT_VERTICAL
wxHTML_INDENT_ALL = htmlc.wxHTML_INDENT_ALL
wxHTML_COND_ISANCHOR = htmlc.wxHTML_COND_ISANCHOR
wxHTML_COND_ISIMAGEMAP = htmlc.wxHTML_COND_ISIMAGEMAP
wxHTML_COND_USER = htmlc.wxHTML_COND_USER
wxPAGE_ODD = htmlc.wxPAGE_ODD
wxPAGE_EVEN = htmlc.wxPAGE_EVEN
wxPAGE_ALL = htmlc.wxPAGE_ALL


#-------------- USER INCLUDE -----------------------


# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxHtmlTagPtr             = wxHtmlTag
wx.wxHtmlParserPtr          = wxHtmlParserPtr
wx.wxHtmlWinParserPtr       = wxHtmlWinParserPtr
wx.wxHtmlTagHandlerPtr      = wxHtmlTagHandlerPtr
wx.wxHtmlWinTagHandlerPtr   = wxHtmlWinTagHandlerPtr
wx.wxHtmlCellPtr            = wxHtmlCellPtr
wx.wxHtmlContainerCellPtr   = wxHtmlContainerCellPtr
wx.wxHtmlWidgetCellPtr      = wxHtmlWidgetCellPtr
wx.HtmlHistoryItemPtr     = HtmlHistoryItemPtr
wx.wxHtmlWindowPtr          = wxHtmlWindowPtr
