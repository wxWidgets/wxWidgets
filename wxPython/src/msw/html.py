# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _html

import windows
import core
wx = core 
#---------------------------------------------------------------------------

HTML_ALIGN_LEFT = _html.HTML_ALIGN_LEFT
HTML_ALIGN_CENTER = _html.HTML_ALIGN_CENTER
HTML_ALIGN_RIGHT = _html.HTML_ALIGN_RIGHT
HTML_ALIGN_BOTTOM = _html.HTML_ALIGN_BOTTOM
HTML_ALIGN_TOP = _html.HTML_ALIGN_TOP
HTML_CLR_FOREGROUND = _html.HTML_CLR_FOREGROUND
HTML_CLR_BACKGROUND = _html.HTML_CLR_BACKGROUND
HTML_UNITS_PIXELS = _html.HTML_UNITS_PIXELS
HTML_UNITS_PERCENT = _html.HTML_UNITS_PERCENT
HTML_INDENT_LEFT = _html.HTML_INDENT_LEFT
HTML_INDENT_RIGHT = _html.HTML_INDENT_RIGHT
HTML_INDENT_TOP = _html.HTML_INDENT_TOP
HTML_INDENT_BOTTOM = _html.HTML_INDENT_BOTTOM
HTML_INDENT_HORIZONTAL = _html.HTML_INDENT_HORIZONTAL
HTML_INDENT_VERTICAL = _html.HTML_INDENT_VERTICAL
HTML_INDENT_ALL = _html.HTML_INDENT_ALL
HTML_COND_ISANCHOR = _html.HTML_COND_ISANCHOR
HTML_COND_ISIMAGEMAP = _html.HTML_COND_ISIMAGEMAP
HTML_COND_USER = _html.HTML_COND_USER
HW_SCROLLBAR_NEVER = _html.HW_SCROLLBAR_NEVER
HW_SCROLLBAR_AUTO = _html.HW_SCROLLBAR_AUTO
HW_NO_SELECTION = _html.HW_NO_SELECTION
HW_DEFAULT_STYLE = _html.HW_DEFAULT_STYLE
HTML_OPEN = _html.HTML_OPEN
HTML_BLOCK = _html.HTML_BLOCK
HTML_REDIRECT = _html.HTML_REDIRECT
HTML_URL_PAGE = _html.HTML_URL_PAGE
HTML_URL_IMAGE = _html.HTML_URL_IMAGE
HTML_URL_OTHER = _html.HTML_URL_OTHER
class HtmlLinkInfo(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlLinkInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHref(*args, **kwargs): return _html.HtmlLinkInfo_GetHref(*args, **kwargs)
    def GetTarget(*args, **kwargs): return _html.HtmlLinkInfo_GetTarget(*args, **kwargs)
    def GetEvent(*args, **kwargs): return _html.HtmlLinkInfo_GetEvent(*args, **kwargs)
    def GetHtmlCell(*args, **kwargs): return _html.HtmlLinkInfo_GetHtmlCell(*args, **kwargs)
    def SetEvent(*args, **kwargs): return _html.HtmlLinkInfo_SetEvent(*args, **kwargs)
    def SetHtmlCell(*args, **kwargs): return _html.HtmlLinkInfo_SetHtmlCell(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlLinkInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlLinkInfoPtr(HtmlLinkInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlLinkInfo
_html.HtmlLinkInfo_swigregister(HtmlLinkInfoPtr)

class HtmlTag(core.Object):
    def GetName(*args, **kwargs): return _html.HtmlTag_GetName(*args, **kwargs)
    def HasParam(*args, **kwargs): return _html.HtmlTag_HasParam(*args, **kwargs)
    def GetParam(*args, **kwargs): return _html.HtmlTag_GetParam(*args, **kwargs)
    def GetAllParams(*args, **kwargs): return _html.HtmlTag_GetAllParams(*args, **kwargs)
    def HasEnding(*args, **kwargs): return _html.HtmlTag_HasEnding(*args, **kwargs)
    def GetBeginPos(*args, **kwargs): return _html.HtmlTag_GetBeginPos(*args, **kwargs)
    def GetEndPos1(*args, **kwargs): return _html.HtmlTag_GetEndPos1(*args, **kwargs)
    def GetEndPos2(*args, **kwargs): return _html.HtmlTag_GetEndPos2(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlTag instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlTagPtr(HtmlTag):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlTag
_html.HtmlTag_swigregister(HtmlTagPtr)

class HtmlParser(core.Object):
    def SetFS(*args, **kwargs): return _html.HtmlParser_SetFS(*args, **kwargs)
    def GetFS(*args, **kwargs): return _html.HtmlParser_GetFS(*args, **kwargs)
    def Parse(*args, **kwargs): return _html.HtmlParser_Parse(*args, **kwargs)
    def InitParser(*args, **kwargs): return _html.HtmlParser_InitParser(*args, **kwargs)
    def DoneParser(*args, **kwargs): return _html.HtmlParser_DoneParser(*args, **kwargs)
    def DoParsing(*args, **kwargs): return _html.HtmlParser_DoParsing(*args, **kwargs)
    def StopParsing(*args, **kwargs): return _html.HtmlParser_StopParsing(*args, **kwargs)
    def AddTagHandler(*args, **kwargs): return _html.HtmlParser_AddTagHandler(*args, **kwargs)
    def GetSource(*args, **kwargs): return _html.HtmlParser_GetSource(*args, **kwargs)
    def PushTagHandler(*args, **kwargs): return _html.HtmlParser_PushTagHandler(*args, **kwargs)
    def PopTagHandler(*args, **kwargs): return _html.HtmlParser_PopTagHandler(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlParserPtr(HtmlParser):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlParser
_html.HtmlParser_swigregister(HtmlParserPtr)

class HtmlWinParser(HtmlParser):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlWinParser(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetDC(*args, **kwargs): return _html.HtmlWinParser_SetDC(*args, **kwargs)
    def GetDC(*args, **kwargs): return _html.HtmlWinParser_GetDC(*args, **kwargs)
    def GetCharHeight(*args, **kwargs): return _html.HtmlWinParser_GetCharHeight(*args, **kwargs)
    def GetCharWidth(*args, **kwargs): return _html.HtmlWinParser_GetCharWidth(*args, **kwargs)
    def GetWindow(*args, **kwargs): return _html.HtmlWinParser_GetWindow(*args, **kwargs)
    def SetFonts(*args, **kwargs): return _html.HtmlWinParser_SetFonts(*args, **kwargs)
    def GetContainer(*args, **kwargs): return _html.HtmlWinParser_GetContainer(*args, **kwargs)
    def OpenContainer(*args, **kwargs): return _html.HtmlWinParser_OpenContainer(*args, **kwargs)
    def SetContainer(*args, **kwargs): return _html.HtmlWinParser_SetContainer(*args, **kwargs)
    def CloseContainer(*args, **kwargs): return _html.HtmlWinParser_CloseContainer(*args, **kwargs)
    def GetFontSize(*args, **kwargs): return _html.HtmlWinParser_GetFontSize(*args, **kwargs)
    def SetFontSize(*args, **kwargs): return _html.HtmlWinParser_SetFontSize(*args, **kwargs)
    def GetFontBold(*args, **kwargs): return _html.HtmlWinParser_GetFontBold(*args, **kwargs)
    def SetFontBold(*args, **kwargs): return _html.HtmlWinParser_SetFontBold(*args, **kwargs)
    def GetFontItalic(*args, **kwargs): return _html.HtmlWinParser_GetFontItalic(*args, **kwargs)
    def SetFontItalic(*args, **kwargs): return _html.HtmlWinParser_SetFontItalic(*args, **kwargs)
    def GetFontUnderlined(*args, **kwargs): return _html.HtmlWinParser_GetFontUnderlined(*args, **kwargs)
    def SetFontUnderlined(*args, **kwargs): return _html.HtmlWinParser_SetFontUnderlined(*args, **kwargs)
    def GetFontFixed(*args, **kwargs): return _html.HtmlWinParser_GetFontFixed(*args, **kwargs)
    def SetFontFixed(*args, **kwargs): return _html.HtmlWinParser_SetFontFixed(*args, **kwargs)
    def GetAlign(*args, **kwargs): return _html.HtmlWinParser_GetAlign(*args, **kwargs)
    def SetAlign(*args, **kwargs): return _html.HtmlWinParser_SetAlign(*args, **kwargs)
    def GetLinkColor(*args, **kwargs): return _html.HtmlWinParser_GetLinkColor(*args, **kwargs)
    def SetLinkColor(*args, **kwargs): return _html.HtmlWinParser_SetLinkColor(*args, **kwargs)
    def GetActualColor(*args, **kwargs): return _html.HtmlWinParser_GetActualColor(*args, **kwargs)
    def SetActualColor(*args, **kwargs): return _html.HtmlWinParser_SetActualColor(*args, **kwargs)
    def SetLink(*args, **kwargs): return _html.HtmlWinParser_SetLink(*args, **kwargs)
    def CreateCurrentFont(*args, **kwargs): return _html.HtmlWinParser_CreateCurrentFont(*args, **kwargs)
    def GetLink(*args, **kwargs): return _html.HtmlWinParser_GetLink(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWinParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlWinParserPtr(HtmlWinParser):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWinParser
_html.HtmlWinParser_swigregister(HtmlWinParserPtr)

class HtmlTagHandler(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlTagHandler)
    def _setCallbackInfo(*args, **kwargs): return _html.HtmlTagHandler__setCallbackInfo(*args, **kwargs)
    def SetParser(*args, **kwargs): return _html.HtmlTagHandler_SetParser(*args, **kwargs)
    def GetParser(*args, **kwargs): return _html.HtmlTagHandler_GetParser(*args, **kwargs)
    def ParseInner(*args, **kwargs): return _html.HtmlTagHandler_ParseInner(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlTagHandlerPtr(HtmlTagHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlTagHandler
_html.HtmlTagHandler_swigregister(HtmlTagHandlerPtr)

class HtmlWinTagHandler(HtmlTagHandler):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlWinTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWinTagHandler)
    def _setCallbackInfo(*args, **kwargs): return _html.HtmlWinTagHandler__setCallbackInfo(*args, **kwargs)
    def SetParser(*args, **kwargs): return _html.HtmlWinTagHandler_SetParser(*args, **kwargs)
    def GetParser(*args, **kwargs): return _html.HtmlWinTagHandler_GetParser(*args, **kwargs)
    def ParseInner(*args, **kwargs): return _html.HtmlWinTagHandler_ParseInner(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlWinTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlWinTagHandlerPtr(HtmlWinTagHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWinTagHandler
_html.HtmlWinTagHandler_swigregister(HtmlWinTagHandlerPtr)


HtmlWinParser_AddTagHandler = _html.HtmlWinParser_AddTagHandler
#---------------------------------------------------------------------------

class HtmlSelection(object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlSelection(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlSelection):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Set(*args, **kwargs): return _html.HtmlSelection_Set(*args, **kwargs)
    def SetCells(*args, **kwargs): return _html.HtmlSelection_SetCells(*args, **kwargs)
    def GetFromCell(*args, **kwargs): return _html.HtmlSelection_GetFromCell(*args, **kwargs)
    def GetToCell(*args, **kwargs): return _html.HtmlSelection_GetToCell(*args, **kwargs)
    def GetFromPos(*args, **kwargs): return _html.HtmlSelection_GetFromPos(*args, **kwargs)
    def GetToPos(*args, **kwargs): return _html.HtmlSelection_GetToPos(*args, **kwargs)
    def GetFromPrivPos(*args, **kwargs): return _html.HtmlSelection_GetFromPrivPos(*args, **kwargs)
    def GetToPrivPos(*args, **kwargs): return _html.HtmlSelection_GetToPrivPos(*args, **kwargs)
    def SetFromPrivPos(*args, **kwargs): return _html.HtmlSelection_SetFromPrivPos(*args, **kwargs)
    def SetToPrivPos(*args, **kwargs): return _html.HtmlSelection_SetToPrivPos(*args, **kwargs)
    def ClearPrivPos(*args, **kwargs): return _html.HtmlSelection_ClearPrivPos(*args, **kwargs)
    def IsEmpty(*args, **kwargs): return _html.HtmlSelection_IsEmpty(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlSelection instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlSelectionPtr(HtmlSelection):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlSelection
_html.HtmlSelection_swigregister(HtmlSelectionPtr)

HTML_SEL_OUT = _html.HTML_SEL_OUT
HTML_SEL_IN = _html.HTML_SEL_IN
HTML_SEL_CHANGING = _html.HTML_SEL_CHANGING
class HtmlRenderingState(object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlRenderingState(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingState):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetSelectionState(*args, **kwargs): return _html.HtmlRenderingState_SetSelectionState(*args, **kwargs)
    def GetSelectionState(*args, **kwargs): return _html.HtmlRenderingState_GetSelectionState(*args, **kwargs)
    def SetFgColour(*args, **kwargs): return _html.HtmlRenderingState_SetFgColour(*args, **kwargs)
    def GetFgColour(*args, **kwargs): return _html.HtmlRenderingState_GetFgColour(*args, **kwargs)
    def SetBgColour(*args, **kwargs): return _html.HtmlRenderingState_SetBgColour(*args, **kwargs)
    def GetBgColour(*args, **kwargs): return _html.HtmlRenderingState_GetBgColour(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingState instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlRenderingStatePtr(HtmlRenderingState):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlRenderingState
_html.HtmlRenderingState_swigregister(HtmlRenderingStatePtr)

class HtmlRenderingStyle(object):
    def GetSelectedTextColour(*args, **kwargs): return _html.HtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)
    def GetSelectedTextBgColour(*args, **kwargs): return _html.HtmlRenderingStyle_GetSelectedTextBgColour(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingStyle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlRenderingStylePtr(HtmlRenderingStyle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlRenderingStyle
_html.HtmlRenderingStyle_swigregister(HtmlRenderingStylePtr)

class DefaultHtmlRenderingStyle(HtmlRenderingStyle):
    def GetSelectedTextColour(*args, **kwargs): return _html.DefaultHtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)
    def GetSelectedTextBgColour(*args, **kwargs): return _html.DefaultHtmlRenderingStyle_GetSelectedTextBgColour(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDefaultHtmlRenderingStyle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DefaultHtmlRenderingStylePtr(DefaultHtmlRenderingStyle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DefaultHtmlRenderingStyle
_html.DefaultHtmlRenderingStyle_swigregister(DefaultHtmlRenderingStylePtr)

class HtmlRenderingInfo(object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlRenderingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingInfo):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetSelection(*args, **kwargs): return _html.HtmlRenderingInfo_SetSelection(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _html.HtmlRenderingInfo_GetSelection(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _html.HtmlRenderingInfo_SetStyle(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _html.HtmlRenderingInfo_GetStyle(*args, **kwargs)
    def GetState(*args, **kwargs): return _html.HtmlRenderingInfo_GetState(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlRenderingInfoPtr(HtmlRenderingInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlRenderingInfo
_html.HtmlRenderingInfo_swigregister(HtmlRenderingInfoPtr)

#---------------------------------------------------------------------------

HTML_FIND_EXACT = _html.HTML_FIND_EXACT
HTML_FIND_NEAREST_BEFORE = _html.HTML_FIND_NEAREST_BEFORE
HTML_FIND_NEAREST_AFTER = _html.HTML_FIND_NEAREST_AFTER
class HtmlCell(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosX(*args, **kwargs): return _html.HtmlCell_GetPosX(*args, **kwargs)
    def GetPosY(*args, **kwargs): return _html.HtmlCell_GetPosY(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _html.HtmlCell_GetWidth(*args, **kwargs)
    def GetHeight(*args, **kwargs): return _html.HtmlCell_GetHeight(*args, **kwargs)
    def GetDescent(*args, **kwargs): return _html.HtmlCell_GetDescent(*args, **kwargs)
    def GetLink(*args, **kwargs): return _html.HtmlCell_GetLink(*args, **kwargs)
    def GetNext(*args, **kwargs): return _html.HtmlCell_GetNext(*args, **kwargs)
    def GetParent(*args, **kwargs): return _html.HtmlCell_GetParent(*args, **kwargs)
    def GetFirstChild(*args, **kwargs): return _html.HtmlCell_GetFirstChild(*args, **kwargs)
    def GetCursor(*args, **kwargs): return _html.HtmlCell_GetCursor(*args, **kwargs)
    def IsFormattingCell(*args, **kwargs): return _html.HtmlCell_IsFormattingCell(*args, **kwargs)
    def SetLink(*args, **kwargs): return _html.HtmlCell_SetLink(*args, **kwargs)
    def SetNext(*args, **kwargs): return _html.HtmlCell_SetNext(*args, **kwargs)
    def SetParent(*args, **kwargs): return _html.HtmlCell_SetParent(*args, **kwargs)
    def SetPos(*args, **kwargs): return _html.HtmlCell_SetPos(*args, **kwargs)
    def Layout(*args, **kwargs): return _html.HtmlCell_Layout(*args, **kwargs)
    def Draw(*args, **kwargs): return _html.HtmlCell_Draw(*args, **kwargs)
    def DrawInvisible(*args, **kwargs): return _html.HtmlCell_DrawInvisible(*args, **kwargs)
    def Find(*args, **kwargs): return _html.HtmlCell_Find(*args, **kwargs)
    def AdjustPagebreak(*args, **kwargs): return _html.HtmlCell_AdjustPagebreak(*args, **kwargs)
    def SetCanLiveOnPagebreak(*args, **kwargs): return _html.HtmlCell_SetCanLiveOnPagebreak(*args, **kwargs)
    def IsLinebreakAllowed(*args, **kwargs): return _html.HtmlCell_IsLinebreakAllowed(*args, **kwargs)
    def IsTerminalCell(*args, **kwargs): return _html.HtmlCell_IsTerminalCell(*args, **kwargs)
    def FindCellByPos(*args, **kwargs): return _html.HtmlCell_FindCellByPos(*args, **kwargs)
    def GetAbsPos(*args, **kwargs): return _html.HtmlCell_GetAbsPos(*args, **kwargs)
    def GetFirstTerminal(*args, **kwargs): return _html.HtmlCell_GetFirstTerminal(*args, **kwargs)
    def GetLastTerminal(*args, **kwargs): return _html.HtmlCell_GetLastTerminal(*args, **kwargs)
    def GetDepth(*args, **kwargs): return _html.HtmlCell_GetDepth(*args, **kwargs)
    def IsBefore(*args, **kwargs): return _html.HtmlCell_IsBefore(*args, **kwargs)
    def ConvertToText(*args, **kwargs): return _html.HtmlCell_ConvertToText(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlCellPtr(HtmlCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlCell
_html.HtmlCell_swigregister(HtmlCellPtr)

class HtmlWordCell(HtmlCell):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlWordCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWordCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlWordCellPtr(HtmlWordCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWordCell
_html.HtmlWordCell_swigregister(HtmlWordCellPtr)

class HtmlContainerCell(HtmlCell):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlContainerCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def InsertCell(*args, **kwargs): return _html.HtmlContainerCell_InsertCell(*args, **kwargs)
    def SetAlignHor(*args, **kwargs): return _html.HtmlContainerCell_SetAlignHor(*args, **kwargs)
    def GetAlignHor(*args, **kwargs): return _html.HtmlContainerCell_GetAlignHor(*args, **kwargs)
    def SetAlignVer(*args, **kwargs): return _html.HtmlContainerCell_SetAlignVer(*args, **kwargs)
    def GetAlignVer(*args, **kwargs): return _html.HtmlContainerCell_GetAlignVer(*args, **kwargs)
    def SetIndent(*args, **kwargs): return _html.HtmlContainerCell_SetIndent(*args, **kwargs)
    def GetIndent(*args, **kwargs): return _html.HtmlContainerCell_GetIndent(*args, **kwargs)
    def GetIndentUnits(*args, **kwargs): return _html.HtmlContainerCell_GetIndentUnits(*args, **kwargs)
    def SetAlign(*args, **kwargs): return _html.HtmlContainerCell_SetAlign(*args, **kwargs)
    def SetWidthFloat(*args, **kwargs): return _html.HtmlContainerCell_SetWidthFloat(*args, **kwargs)
    def SetWidthFloatFromTag(*args, **kwargs): return _html.HtmlContainerCell_SetWidthFloatFromTag(*args, **kwargs)
    def SetMinHeight(*args, **kwargs): return _html.HtmlContainerCell_SetMinHeight(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _html.HtmlContainerCell_SetBackgroundColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _html.HtmlContainerCell_GetBackgroundColour(*args, **kwargs)
    def SetBorder(*args, **kwargs): return _html.HtmlContainerCell_SetBorder(*args, **kwargs)
    def GetFirstChild(*args, **kwargs): return _html.HtmlContainerCell_GetFirstChild(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlContainerCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlContainerCellPtr(HtmlContainerCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlContainerCell
_html.HtmlContainerCell_swigregister(HtmlContainerCellPtr)

class HtmlColourCell(HtmlCell):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlColourCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlColourCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlColourCellPtr(HtmlColourCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlColourCell
_html.HtmlColourCell_swigregister(HtmlColourCellPtr)

class HtmlFontCell(HtmlCell):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlFontCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlFontCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlFontCellPtr(HtmlFontCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlFontCell
_html.HtmlFontCell_swigregister(HtmlFontCellPtr)

class HtmlWidgetCell(HtmlCell):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlWidgetCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWidgetCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlWidgetCellPtr(HtmlWidgetCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWidgetCell
_html.HtmlWidgetCell_swigregister(HtmlWidgetCellPtr)

#---------------------------------------------------------------------------

class HtmlFilter(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlFilter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlFilter)
    def _setCallbackInfo(*args, **kwargs): return _html.HtmlFilter__setCallbackInfo(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlFilter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlFilterPtr(HtmlFilter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlFilter
_html.HtmlFilter_swigregister(HtmlFilterPtr)

#---------------------------------------------------------------------------

class HtmlWindow(windows.ScrolledWindow):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWindow); self._setOORInfo(self)
    def Create(*args, **kwargs): return _html.HtmlWindow_Create(*args, **kwargs)
    def _setCallbackInfo(*args, **kwargs): return _html.HtmlWindow__setCallbackInfo(*args, **kwargs)
    def SetPage(*args, **kwargs): return _html.HtmlWindow_SetPage(*args, **kwargs)
    def LoadPage(*args, **kwargs): return _html.HtmlWindow_LoadPage(*args, **kwargs)
    def LoadFile(*args, **kwargs): return _html.HtmlWindow_LoadFile(*args, **kwargs)
    def AppendToPage(*args, **kwargs): return _html.HtmlWindow_AppendToPage(*args, **kwargs)
    def GetOpenedPage(*args, **kwargs): return _html.HtmlWindow_GetOpenedPage(*args, **kwargs)
    def GetOpenedAnchor(*args, **kwargs): return _html.HtmlWindow_GetOpenedAnchor(*args, **kwargs)
    def GetOpenedPageTitle(*args, **kwargs): return _html.HtmlWindow_GetOpenedPageTitle(*args, **kwargs)
    def SetRelatedFrame(*args, **kwargs): return _html.HtmlWindow_SetRelatedFrame(*args, **kwargs)
    def GetRelatedFrame(*args, **kwargs): return _html.HtmlWindow_GetRelatedFrame(*args, **kwargs)
    def SetRelatedStatusBar(*args, **kwargs): return _html.HtmlWindow_SetRelatedStatusBar(*args, **kwargs)
    def SetFonts(*args, **kwargs): return _html.HtmlWindow_SetFonts(*args, **kwargs)
    def SetTitle(*args, **kwargs): return _html.HtmlWindow_SetTitle(*args, **kwargs)
    def SetBorders(*args, **kwargs): return _html.HtmlWindow_SetBorders(*args, **kwargs)
    def ReadCustomization(*args, **kwargs): return _html.HtmlWindow_ReadCustomization(*args, **kwargs)
    def WriteCustomization(*args, **kwargs): return _html.HtmlWindow_WriteCustomization(*args, **kwargs)
    def HistoryBack(*args, **kwargs): return _html.HtmlWindow_HistoryBack(*args, **kwargs)
    def HistoryForward(*args, **kwargs): return _html.HtmlWindow_HistoryForward(*args, **kwargs)
    def HistoryCanBack(*args, **kwargs): return _html.HtmlWindow_HistoryCanBack(*args, **kwargs)
    def HistoryCanForward(*args, **kwargs): return _html.HtmlWindow_HistoryCanForward(*args, **kwargs)
    def HistoryClear(*args, **kwargs): return _html.HtmlWindow_HistoryClear(*args, **kwargs)
    def GetInternalRepresentation(*args, **kwargs): return _html.HtmlWindow_GetInternalRepresentation(*args, **kwargs)
    def GetParser(*args, **kwargs): return _html.HtmlWindow_GetParser(*args, **kwargs)
    def ScrollToAnchor(*args, **kwargs): return _html.HtmlWindow_ScrollToAnchor(*args, **kwargs)
    def HasAnchor(*args, **kwargs): return _html.HtmlWindow_HasAnchor(*args, **kwargs)
    AddFilter = staticmethod(_html.HtmlWindow_AddFilter)
    def base_OnLinkClicked(*args, **kwargs): return _html.HtmlWindow_base_OnLinkClicked(*args, **kwargs)
    def base_OnSetTitle(*args, **kwargs): return _html.HtmlWindow_base_OnSetTitle(*args, **kwargs)
    def base_OnCellMouseHover(*args, **kwargs): return _html.HtmlWindow_base_OnCellMouseHover(*args, **kwargs)
    def base_OnCellClicked(*args, **kwargs): return _html.HtmlWindow_base_OnCellClicked(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlWindowPtr(HtmlWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWindow
_html.HtmlWindow_swigregister(HtmlWindowPtr)

def PreHtmlWindow(*args, **kwargs):
    val = _html.new_PreHtmlWindow(*args, **kwargs)
    val.thisown = 1
    val._setOORInfo(val)
    return val

HtmlWindow_AddFilter = _html.HtmlWindow_AddFilter

#---------------------------------------------------------------------------

class HtmlDCRenderer(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlDCRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlDCRenderer):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetDC(*args, **kwargs): return _html.HtmlDCRenderer_SetDC(*args, **kwargs)
    def SetSize(*args, **kwargs): return _html.HtmlDCRenderer_SetSize(*args, **kwargs)
    def SetHtmlText(*args, **kwargs): return _html.HtmlDCRenderer_SetHtmlText(*args, **kwargs)
    def SetFonts(*args, **kwargs): return _html.HtmlDCRenderer_SetFonts(*args, **kwargs)
    def Render(*args, **kwargs): return _html.HtmlDCRenderer_Render(*args, **kwargs)
    def GetTotalHeight(*args, **kwargs): return _html.HtmlDCRenderer_GetTotalHeight(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlDCRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlDCRendererPtr(HtmlDCRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlDCRenderer
_html.HtmlDCRenderer_swigregister(HtmlDCRendererPtr)

PAGE_ODD = _html.PAGE_ODD
PAGE_EVEN = _html.PAGE_EVEN
PAGE_ALL = _html.PAGE_ALL
class HtmlPrintout(windows.Printout):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlPrintout(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetHtmlText(*args, **kwargs): return _html.HtmlPrintout_SetHtmlText(*args, **kwargs)
    def SetHtmlFile(*args, **kwargs): return _html.HtmlPrintout_SetHtmlFile(*args, **kwargs)
    def SetHeader(*args, **kwargs): return _html.HtmlPrintout_SetHeader(*args, **kwargs)
    def SetFooter(*args, **kwargs): return _html.HtmlPrintout_SetFooter(*args, **kwargs)
    def SetFonts(*args, **kwargs): return _html.HtmlPrintout_SetFonts(*args, **kwargs)
    def SetMargins(*args, **kwargs): return _html.HtmlPrintout_SetMargins(*args, **kwargs)
    AddFilter = staticmethod(_html.HtmlPrintout_AddFilter)
    CleanUpStatics = staticmethod(_html.HtmlPrintout_CleanUpStatics)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlPrintout instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlPrintoutPtr(HtmlPrintout):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlPrintout
_html.HtmlPrintout_swigregister(HtmlPrintoutPtr)

HtmlPrintout_AddFilter = _html.HtmlPrintout_AddFilter

HtmlPrintout_CleanUpStatics = _html.HtmlPrintout_CleanUpStatics

class HtmlEasyPrinting(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlEasyPrinting(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlEasyPrinting):
        try:
            if self.thisown: destroy(self)
        except: pass
    def PreviewFile(*args, **kwargs): return _html.HtmlEasyPrinting_PreviewFile(*args, **kwargs)
    def PreviewText(*args, **kwargs): return _html.HtmlEasyPrinting_PreviewText(*args, **kwargs)
    def PrintFile(*args, **kwargs): return _html.HtmlEasyPrinting_PrintFile(*args, **kwargs)
    def PrintText(*args, **kwargs): return _html.HtmlEasyPrinting_PrintText(*args, **kwargs)
    def PrinterSetup(*args, **kwargs): return _html.HtmlEasyPrinting_PrinterSetup(*args, **kwargs)
    def PageSetup(*args, **kwargs): return _html.HtmlEasyPrinting_PageSetup(*args, **kwargs)
    def SetHeader(*args, **kwargs): return _html.HtmlEasyPrinting_SetHeader(*args, **kwargs)
    def SetFooter(*args, **kwargs): return _html.HtmlEasyPrinting_SetFooter(*args, **kwargs)
    def SetFonts(*args, **kwargs): return _html.HtmlEasyPrinting_SetFonts(*args, **kwargs)
    def GetPrintData(*args, **kwargs): return _html.HtmlEasyPrinting_GetPrintData(*args, **kwargs)
    def GetPageSetupData(*args, **kwargs): return _html.HtmlEasyPrinting_GetPageSetupData(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlEasyPrinting instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlEasyPrintingPtr(HtmlEasyPrinting):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlEasyPrinting
_html.HtmlEasyPrinting_swigregister(HtmlEasyPrintingPtr)

#---------------------------------------------------------------------------

class HtmlBookRecord(object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlBookRecord(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetBookFile(*args, **kwargs): return _html.HtmlBookRecord_GetBookFile(*args, **kwargs)
    def GetTitle(*args, **kwargs): return _html.HtmlBookRecord_GetTitle(*args, **kwargs)
    def GetStart(*args, **kwargs): return _html.HtmlBookRecord_GetStart(*args, **kwargs)
    def GetBasePath(*args, **kwargs): return _html.HtmlBookRecord_GetBasePath(*args, **kwargs)
    def SetContentsRange(*args, **kwargs): return _html.HtmlBookRecord_SetContentsRange(*args, **kwargs)
    def GetContentsStart(*args, **kwargs): return _html.HtmlBookRecord_GetContentsStart(*args, **kwargs)
    def GetContentsEnd(*args, **kwargs): return _html.HtmlBookRecord_GetContentsEnd(*args, **kwargs)
    def SetTitle(*args, **kwargs): return _html.HtmlBookRecord_SetTitle(*args, **kwargs)
    def SetBasePath(*args, **kwargs): return _html.HtmlBookRecord_SetBasePath(*args, **kwargs)
    def SetStart(*args, **kwargs): return _html.HtmlBookRecord_SetStart(*args, **kwargs)
    def GetFullPath(*args, **kwargs): return _html.HtmlBookRecord_GetFullPath(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlBookRecord instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlBookRecordPtr(HtmlBookRecord):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlBookRecord
_html.HtmlBookRecord_swigregister(HtmlBookRecordPtr)

class HtmlContentsItem(object):
    def GetLevel(*args, **kwargs): return _html.HtmlContentsItem_GetLevel(*args, **kwargs)
    def GetID(*args, **kwargs): return _html.HtmlContentsItem_GetID(*args, **kwargs)
    def GetName(*args, **kwargs): return _html.HtmlContentsItem_GetName(*args, **kwargs)
    def GetPage(*args, **kwargs): return _html.HtmlContentsItem_GetPage(*args, **kwargs)
    def GetBook(*args, **kwargs): return _html.HtmlContentsItem_GetBook(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlContentsItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlContentsItemPtr(HtmlContentsItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlContentsItem
_html.HtmlContentsItem_swigregister(HtmlContentsItemPtr)

class HtmlSearchStatus(object):
    def Search(*args, **kwargs): return _html.HtmlSearchStatus_Search(*args, **kwargs)
    def IsActive(*args, **kwargs): return _html.HtmlSearchStatus_IsActive(*args, **kwargs)
    def GetCurIndex(*args, **kwargs): return _html.HtmlSearchStatus_GetCurIndex(*args, **kwargs)
    def GetMaxIndex(*args, **kwargs): return _html.HtmlSearchStatus_GetMaxIndex(*args, **kwargs)
    def GetName(*args, **kwargs): return _html.HtmlSearchStatus_GetName(*args, **kwargs)
    def GetContentsItem(*args, **kwargs): return _html.HtmlSearchStatus_GetContentsItem(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlSearchStatus instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlSearchStatusPtr(HtmlSearchStatus):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlSearchStatus
_html.HtmlSearchStatus_swigregister(HtmlSearchStatusPtr)

class HtmlHelpData(object):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlHelpData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlHelpData):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetTempDir(*args, **kwargs): return _html.HtmlHelpData_SetTempDir(*args, **kwargs)
    def AddBook(*args, **kwargs): return _html.HtmlHelpData_AddBook(*args, **kwargs)
    def FindPageByName(*args, **kwargs): return _html.HtmlHelpData_FindPageByName(*args, **kwargs)
    def FindPageById(*args, **kwargs): return _html.HtmlHelpData_FindPageById(*args, **kwargs)
    def GetBookRecArray(*args, **kwargs): return _html.HtmlHelpData_GetBookRecArray(*args, **kwargs)
    def GetContents(*args, **kwargs): return _html.HtmlHelpData_GetContents(*args, **kwargs)
    def GetContentsCnt(*args, **kwargs): return _html.HtmlHelpData_GetContentsCnt(*args, **kwargs)
    def GetIndex(*args, **kwargs): return _html.HtmlHelpData_GetIndex(*args, **kwargs)
    def GetIndexCnt(*args, **kwargs): return _html.HtmlHelpData_GetIndexCnt(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlHelpDataPtr(HtmlHelpData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpData
_html.HtmlHelpData_swigregister(HtmlHelpDataPtr)

class HtmlHelpFrame(windows.Frame):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlHelpFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetData(*args, **kwargs): return _html.HtmlHelpFrame_GetData(*args, **kwargs)
    def SetTitleFormat(*args, **kwargs): return _html.HtmlHelpFrame_SetTitleFormat(*args, **kwargs)
    def Display(*args, **kwargs): return _html.HtmlHelpFrame_Display(*args, **kwargs)
    def DisplayID(*args, **kwargs): return _html.HtmlHelpFrame_DisplayID(*args, **kwargs)
    def DisplayContents(*args, **kwargs): return _html.HtmlHelpFrame_DisplayContents(*args, **kwargs)
    def DisplayIndex(*args, **kwargs): return _html.HtmlHelpFrame_DisplayIndex(*args, **kwargs)
    def KeywordSearch(*args, **kwargs): return _html.HtmlHelpFrame_KeywordSearch(*args, **kwargs)
    def UseConfig(*args, **kwargs): return _html.HtmlHelpFrame_UseConfig(*args, **kwargs)
    def ReadCustomization(*args, **kwargs): return _html.HtmlHelpFrame_ReadCustomization(*args, **kwargs)
    def WriteCustomization(*args, **kwargs): return _html.HtmlHelpFrame_WriteCustomization(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlHelpFramePtr(HtmlHelpFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpFrame
_html.HtmlHelpFrame_swigregister(HtmlHelpFramePtr)

HF_TOOLBAR = _html.HF_TOOLBAR
HF_FLATTOOLBAR = _html.HF_FLATTOOLBAR
HF_CONTENTS = _html.HF_CONTENTS
HF_INDEX = _html.HF_INDEX
HF_SEARCH = _html.HF_SEARCH
HF_BOOKMARKS = _html.HF_BOOKMARKS
HF_OPENFILES = _html.HF_OPENFILES
HF_PRINT = _html.HF_PRINT
HF_DEFAULTSTYLE = _html.HF_DEFAULTSTYLE
class HtmlHelpController(core.EvtHandler):
    def __init__(self, *args, **kwargs):
        newobj = _html.new_HtmlHelpController(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __del__(self, destroy=_html.delete_HtmlHelpController):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetTitleFormat(*args, **kwargs): return _html.HtmlHelpController_SetTitleFormat(*args, **kwargs)
    def SetTempDir(*args, **kwargs): return _html.HtmlHelpController_SetTempDir(*args, **kwargs)
    def AddBook(*args, **kwargs): return _html.HtmlHelpController_AddBook(*args, **kwargs)
    def Display(*args, **kwargs): return _html.HtmlHelpController_Display(*args, **kwargs)
    def DisplayID(*args, **kwargs): return _html.HtmlHelpController_DisplayID(*args, **kwargs)
    def DisplayContents(*args, **kwargs): return _html.HtmlHelpController_DisplayContents(*args, **kwargs)
    def DisplayIndex(*args, **kwargs): return _html.HtmlHelpController_DisplayIndex(*args, **kwargs)
    def KeywordSearch(*args, **kwargs): return _html.HtmlHelpController_KeywordSearch(*args, **kwargs)
    def UseConfig(*args, **kwargs): return _html.HtmlHelpController_UseConfig(*args, **kwargs)
    def ReadCustomization(*args, **kwargs): return _html.HtmlHelpController_ReadCustomization(*args, **kwargs)
    def WriteCustomization(*args, **kwargs): return _html.HtmlHelpController_WriteCustomization(*args, **kwargs)
    def GetFrame(*args, **kwargs): return _html.HtmlHelpController_GetFrame(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpController instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HtmlHelpControllerPtr(HtmlHelpController):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpController
_html.HtmlHelpController_swigregister(HtmlHelpControllerPtr)


