# This file was created automatically by SWIG.
import htmlc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

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

from filesys import *
import wx
class wxHtmlLinkInfoPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetHref(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_GetHref(self, *_args, **_kwargs)
        return val
    def GetTarget(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_GetTarget(self, *_args, **_kwargs)
        return val
    def GetEvent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_GetEvent(self, *_args, **_kwargs)
        return val
    def GetHtmlCell(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_GetHtmlCell(self, *_args, **_kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def SetEvent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_SetEvent(self, *_args, **_kwargs)
        return val
    def SetHtmlCell(self, *_args, **_kwargs):
        val = htmlc.wxHtmlLinkInfo_SetHtmlCell(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlLinkInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlLinkInfo(wxHtmlLinkInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlLinkInfo(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlTagPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetName(self, *_args, **_kwargs)
        return val
    def HasParam(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_HasParam(self, *_args, **_kwargs)
        return val
    def GetParam(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetParam(self, *_args, **_kwargs)
        return val
    def GetAllParams(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetAllParams(self, *_args, **_kwargs)
        return val
    def HasEnding(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_HasEnding(self, *_args, **_kwargs)
        return val
    def GetBeginPos(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetBeginPos(self, *_args, **_kwargs)
        return val
    def GetEndPos1(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetEndPos1(self, *_args, **_kwargs)
        return val
    def GetEndPos2(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTag_GetEndPos2(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlTag instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlTag(wxHtmlTagPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlParserPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetFS(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_SetFS(self, *_args, **_kwargs)
        return val
    def GetFS(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_GetFS(self, *_args, **_kwargs)
        return val
    def Parse(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_Parse(self, *_args, **_kwargs)
        return val
    def InitParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_InitParser(self, *_args, **_kwargs)
        return val
    def DoneParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_DoneParser(self, *_args, **_kwargs)
        return val
    def DoParsing(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_DoParsing(self, *_args, **_kwargs)
        return val
    def StopParsing(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_StopParsing(self, *_args, **_kwargs)
        return val
    def AddTagHandler(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_AddTagHandler(self, *_args, **_kwargs)
        return val
    def GetSource(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_GetSource(self, *_args, **_kwargs)
        return val
    def PushTagHandler(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_PushTagHandler(self, *_args, **_kwargs)
        return val
    def PopTagHandler(self, *_args, **_kwargs):
        val = htmlc.wxHtmlParser_PopTagHandler(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlParser(wxHtmlParserPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlWinParserPtr(wxHtmlParserPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetDC(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetDC(self, *_args, **_kwargs)
        return val
    def GetDC(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetDC(self, *_args, **_kwargs)
        return val
    def GetCharHeight(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetCharHeight(self, *_args, **_kwargs)
        return val
    def GetCharWidth(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetCharWidth(self, *_args, **_kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetWindow(self, *_args, **_kwargs)
        if val: val = wxHtmlWindowPtr(val) 
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFonts(self, *_args, **_kwargs)
        return val
    def GetContainer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetContainer(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def OpenContainer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_OpenContainer(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def SetContainer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetContainer(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def CloseContainer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_CloseContainer(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def GetFontSize(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetFontSize(self, *_args, **_kwargs)
        return val
    def SetFontSize(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFontSize(self, *_args, **_kwargs)
        return val
    def GetFontBold(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetFontBold(self, *_args, **_kwargs)
        return val
    def SetFontBold(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFontBold(self, *_args, **_kwargs)
        return val
    def GetFontItalic(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetFontItalic(self, *_args, **_kwargs)
        return val
    def SetFontItalic(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFontItalic(self, *_args, **_kwargs)
        return val
    def GetFontUnderlined(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetFontUnderlined(self, *_args, **_kwargs)
        return val
    def SetFontUnderlined(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFontUnderlined(self, *_args, **_kwargs)
        return val
    def GetFontFixed(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetFontFixed(self, *_args, **_kwargs)
        return val
    def SetFontFixed(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetFontFixed(self, *_args, **_kwargs)
        return val
    def GetAlign(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetAlign(self, *_args, **_kwargs)
        return val
    def SetAlign(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetAlign(self, *_args, **_kwargs)
        return val
    def GetLinkColor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetLinkColor(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetLinkColor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetLinkColor(self, *_args, **_kwargs)
        return val
    def GetActualColor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetActualColor(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetActualColor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetActualColor(self, *_args, **_kwargs)
        return val
    def SetLink(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_SetLink(self, *_args, **_kwargs)
        return val
    def CreateCurrentFont(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_CreateCurrentFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetLink(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinParser_GetLink(self, *_args, **_kwargs)
        if val: val = wxHtmlLinkInfoPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlWinParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlWinParser(wxHtmlWinParserPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlWinParser(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlTagHandlerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTagHandler__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def SetParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTagHandler_SetParser(self, *_args, **_kwargs)
        return val
    def GetParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTagHandler_GetParser(self, *_args, **_kwargs)
        if val: val = wxHtmlParserPtr(val) 
        return val
    def ParseInner(self, *_args, **_kwargs):
        val = htmlc.wxHtmlTagHandler_ParseInner(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlTagHandler(wxHtmlTagHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlTagHandler(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxHtmlTagHandler)




class wxHtmlWinTagHandlerPtr(wxHtmlTagHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinTagHandler__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def SetParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinTagHandler_SetParser(self, *_args, **_kwargs)
        return val
    def GetParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinTagHandler_GetParser(self, *_args, **_kwargs)
        if val: val = wxHtmlWinParserPtr(val) 
        return val
    def ParseInner(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWinTagHandler_ParseInner(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlWinTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlWinTagHandler(wxHtmlWinTagHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlWinTagHandler(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxHtmlWinTagHandler)




class wxHtmlCellPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosX(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetPosX(self, *_args, **_kwargs)
        return val
    def GetPosY(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetPosY(self, *_args, **_kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetWidth(self, *_args, **_kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetHeight(self, *_args, **_kwargs)
        return val
    def GetDescent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetDescent(self, *_args, **_kwargs)
        return val
    def GetLink(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetLink(self, *_args, **_kwargs)
        if val: val = wxHtmlLinkInfoPtr(val) 
        return val
    def GetNext(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetNext(self, *_args, **_kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def GetParent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_GetParent(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def SetLink(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_SetLink(self, *_args, **_kwargs)
        return val
    def SetNext(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_SetNext(self, *_args, **_kwargs)
        return val
    def SetParent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_SetParent(self, *_args, **_kwargs)
        return val
    def SetPos(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_SetPos(self, *_args, **_kwargs)
        return val
    def Layout(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_Layout(self, *_args, **_kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_Draw(self, *_args, **_kwargs)
        return val
    def DrawInvisible(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_DrawInvisible(self, *_args, **_kwargs)
        return val
    def Find(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_Find(self, *_args, **_kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def AdjustPagebreak(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_AdjustPagebreak(self, *_args, **_kwargs)
        return val
    def SetCanLiveOnPagebreak(self, *_args, **_kwargs):
        val = htmlc.wxHtmlCell_SetCanLiveOnPagebreak(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlCell(wxHtmlCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlWordCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlWordCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlWordCell(wxHtmlWordCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlWordCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlContainerCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def InsertCell(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_InsertCell(self, *_args, **_kwargs)
        return val
    def SetAlignHor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetAlignHor(self, *_args, **_kwargs)
        return val
    def GetAlignHor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetAlignHor(self, *_args, **_kwargs)
        return val
    def SetAlignVer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetAlignVer(self, *_args, **_kwargs)
        return val
    def GetAlignVer(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetAlignVer(self, *_args, **_kwargs)
        return val
    def SetIndent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetIndent(self, *_args, **_kwargs)
        return val
    def GetIndent(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetIndent(self, *_args, **_kwargs)
        return val
    def GetIndentUnits(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetIndentUnits(self, *_args, **_kwargs)
        return val
    def SetAlign(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetAlign(self, *_args, **_kwargs)
        return val
    def SetWidthFloat(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetWidthFloat(self, *_args, **_kwargs)
        return val
    def SetWidthFloatFromTag(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetWidthFloatFromTag(self, *_args, **_kwargs)
        return val
    def SetMinHeight(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetMinHeight(self, *_args, **_kwargs)
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetBackgroundColour(self, *_args, **_kwargs)
        return val
    def GetBackgroundColour(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_SetBorder(self, *_args, **_kwargs)
        return val
    def GetFirstCell(self, *_args, **_kwargs):
        val = htmlc.wxHtmlContainerCell_GetFirstCell(self, *_args, **_kwargs)
        if val: val = wxHtmlCellPtr(val) 
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlContainerCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlContainerCell(wxHtmlContainerCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlContainerCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlColourCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlColourCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlColourCell(wxHtmlColourCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlColourCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlFontCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlFontCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlFontCell(wxHtmlFontCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlFontCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlWidgetCellPtr(wxHtmlCellPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlWidgetCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlWidgetCell(wxHtmlWidgetCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlWidgetCell(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlFilterPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = htmlc.wxHtmlFilter__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlFilter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlFilter(wxHtmlFilterPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlFilter(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxHtmlFilter)




class wxHtmlWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_Create(self, *_args, **_kwargs)
        return val
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def SetPage(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetPage(self, *_args, **_kwargs)
        return val
    def LoadPage(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_LoadPage(self, *_args, **_kwargs)
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_LoadFile(self, *_args, **_kwargs)
        return val
    def AppendToPage(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_AppendToPage(self, *_args, **_kwargs)
        return val
    def GetOpenedPage(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetOpenedPage(self, *_args, **_kwargs)
        return val
    def GetOpenedAnchor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetOpenedAnchor(self, *_args, **_kwargs)
        return val
    def GetOpenedPageTitle(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetOpenedPageTitle(self, *_args, **_kwargs)
        return val
    def SetRelatedFrame(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetRelatedFrame(self, *_args, **_kwargs)
        return val
    def GetRelatedFrame(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetRelatedFrame(self, *_args, **_kwargs)
        return val
    def SetRelatedStatusBar(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetRelatedStatusBar(self, *_args, **_kwargs)
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetFonts(self, *_args, **_kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetTitle(self, *_args, **_kwargs)
        return val
    def SetBorders(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_SetBorders(self, *_args, **_kwargs)
        return val
    def ReadCustomization(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_ReadCustomization(self, *_args, **_kwargs)
        return val
    def WriteCustomization(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_WriteCustomization(self, *_args, **_kwargs)
        return val
    def HistoryBack(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HistoryBack(self, *_args, **_kwargs)
        return val
    def HistoryForward(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HistoryForward(self, *_args, **_kwargs)
        return val
    def HistoryCanBack(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HistoryCanBack(self, *_args, **_kwargs)
        return val
    def HistoryCanForward(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HistoryCanForward(self, *_args, **_kwargs)
        return val
    def HistoryClear(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HistoryClear(self, *_args, **_kwargs)
        return val
    def GetInternalRepresentation(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetInternalRepresentation(self, *_args, **_kwargs)
        if val: val = wxHtmlContainerCellPtr(val) 
        return val
    def GetParser(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_GetParser(self, *_args, **_kwargs)
        if val: val = wxHtmlWinParserPtr(val) 
        return val
    def ScrollToAnchor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_ScrollToAnchor(self, *_args, **_kwargs)
        return val
    def HasAnchor(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_HasAnchor(self, *_args, **_kwargs)
        return val
    def base_OnLinkClicked(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_base_OnLinkClicked(self, *_args, **_kwargs)
        return val
    def base_OnSetTitle(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_base_OnSetTitle(self, *_args, **_kwargs)
        return val
    def base_OnCellMouseHover(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_base_OnCellMouseHover(self, *_args, **_kwargs)
        return val
    def base_OnCellClicked(self, *_args, **_kwargs):
        val = htmlc.wxHtmlWindow_base_OnCellClicked(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlWindow(wxHtmlWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxHtmlWindow)
        self._setOORInfo(self)



def wxPreHtmlWindow(*_args,**_kwargs):
    val = wxHtmlWindowPtr(htmlc.new_wxPreHtmlWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxHtmlDCRendererPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=htmlc.delete_wxHtmlDCRenderer):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def SetDC(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_SetDC(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_SetSize(self, *_args, **_kwargs)
        return val
    def SetHtmlText(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_SetHtmlText(self, *_args, **_kwargs)
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_SetFonts(self, *_args, **_kwargs)
        return val
    def Render(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_Render(self, *_args, **_kwargs)
        return val
    def GetTotalHeight(self, *_args, **_kwargs):
        val = htmlc.wxHtmlDCRenderer_GetTotalHeight(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlDCRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlDCRenderer(wxHtmlDCRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlDCRenderer(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlPrintoutPtr(wxPrintoutPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetHtmlText(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetHtmlText(self, *_args, **_kwargs)
        return val
    def SetHtmlFile(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetHtmlFile(self, *_args, **_kwargs)
        return val
    def SetHeader(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetHeader(self, *_args, **_kwargs)
        return val
    def SetFooter(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetFooter(self, *_args, **_kwargs)
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetFonts(self, *_args, **_kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = htmlc.wxHtmlPrintout_SetMargins(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlPrintout instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlPrintout(wxHtmlPrintoutPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlPrintout(*_args,**_kwargs)
        self.thisown = 1




class wxHtmlEasyPrintingPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=htmlc.delete_wxHtmlEasyPrinting):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def PreviewFile(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PreviewFile(self, *_args, **_kwargs)
        return val
    def PreviewText(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PreviewText(self, *_args, **_kwargs)
        return val
    def PrintFile(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PrintFile(self, *_args, **_kwargs)
        return val
    def PrintText(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PrintText(self, *_args, **_kwargs)
        return val
    def PrinterSetup(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PrinterSetup(self, *_args, **_kwargs)
        return val
    def PageSetup(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_PageSetup(self, *_args, **_kwargs)
        return val
    def SetHeader(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_SetHeader(self, *_args, **_kwargs)
        return val
    def SetFooter(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_SetFooter(self, *_args, **_kwargs)
        return val
    def GetPrintData(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_GetPrintData(self, *_args, **_kwargs)
        if val: val = wxPrintDataPtr(val) 
        return val
    def GetPageSetupData(self, *_args, **_kwargs):
        val = htmlc.wxHtmlEasyPrinting_GetPageSetupData(self, *_args, **_kwargs)
        if val: val = wxPageSetupDialogDataPtr(val) 
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlEasyPrinting instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlEasyPrinting(wxHtmlEasyPrintingPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = htmlc.new_wxHtmlEasyPrinting(*_args,**_kwargs)
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
wxHW_SCROLLBAR_NEVER = htmlc.wxHW_SCROLLBAR_NEVER
wxHW_SCROLLBAR_AUTO = htmlc.wxHW_SCROLLBAR_AUTO
wxHTML_OPEN = htmlc.wxHTML_OPEN
wxHTML_BLOCK = htmlc.wxHTML_BLOCK
wxHTML_REDIRECT = htmlc.wxHTML_REDIRECT
wxHTML_URL_PAGE = htmlc.wxHTML_URL_PAGE
wxHTML_URL_IMAGE = htmlc.wxHTML_URL_IMAGE
wxHTML_URL_OTHER = htmlc.wxHTML_URL_OTHER
wxPAGE_ODD = htmlc.wxPAGE_ODD
wxPAGE_EVEN = htmlc.wxPAGE_EVEN
wxPAGE_ALL = htmlc.wxPAGE_ALL


#-------------- USER INCLUDE -----------------------


# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxHtmlTagPtr             = wxHtmlTagPtr
wx.wxHtmlParserPtr          = wxHtmlParserPtr
wx.wxHtmlWinParserPtr       = wxHtmlWinParserPtr
wx.wxHtmlTagHandlerPtr      = wxHtmlTagHandlerPtr
wx.wxHtmlWinTagHandlerPtr   = wxHtmlWinTagHandlerPtr
wx.wxHtmlCellPtr            = wxHtmlCellPtr
wx.wxHtmlContainerCellPtr   = wxHtmlContainerCellPtr
wx.wxHtmlWidgetCellPtr      = wxHtmlWidgetCellPtr
wx.wxHtmlWindowPtr          = wxHtmlWindowPtr
wx.wxHtmlLinkInfoPtr        = wxHtmlLinkInfoPtr
wx.wxHtmlFilterPtr          = wxHtmlFilterPtr

