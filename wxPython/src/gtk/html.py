# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
Classes for a simple HTML rendering window, HTML Help Window, etc.
"""

import _html

import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
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
HTML_FONT_SIZE_1 = _html.HTML_FONT_SIZE_1
HTML_FONT_SIZE_2 = _html.HTML_FONT_SIZE_2
HTML_FONT_SIZE_3 = _html.HTML_FONT_SIZE_3
HTML_FONT_SIZE_4 = _html.HTML_FONT_SIZE_4
HTML_FONT_SIZE_5 = _html.HTML_FONT_SIZE_5
HTML_FONT_SIZE_6 = _html.HTML_FONT_SIZE_6
HTML_FONT_SIZE_7 = _html.HTML_FONT_SIZE_7
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
class HtmlLinkInfo(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlLinkInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String href, String target=EmptyString) -> HtmlLinkInfo"""
        newobj = _html.new_HtmlLinkInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHref(*args, **kwargs):
        """GetHref(self) -> String"""
        return _html.HtmlLinkInfo_GetHref(*args, **kwargs)

    def GetTarget(*args, **kwargs):
        """GetTarget(self) -> String"""
        return _html.HtmlLinkInfo_GetTarget(*args, **kwargs)

    def GetEvent(*args, **kwargs):
        """GetEvent(self) -> MouseEvent"""
        return _html.HtmlLinkInfo_GetEvent(*args, **kwargs)

    def GetHtmlCell(*args, **kwargs):
        """GetHtmlCell(self) -> HtmlCell"""
        return _html.HtmlLinkInfo_GetHtmlCell(*args, **kwargs)

    def SetEvent(*args, **kwargs):
        """SetEvent(self, MouseEvent e)"""
        return _html.HtmlLinkInfo_SetEvent(*args, **kwargs)

    def SetHtmlCell(*args, **kwargs):
        """SetHtmlCell(self, HtmlCell e)"""
        return _html.HtmlLinkInfo_SetHtmlCell(*args, **kwargs)


class HtmlLinkInfoPtr(HtmlLinkInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlLinkInfo
_html.HtmlLinkInfo_swigregister(HtmlLinkInfoPtr)
cvar = _html.cvar
HtmlWindowNameStr = cvar.HtmlWindowNameStr
HtmlPrintoutTitleStr = cvar.HtmlPrintoutTitleStr
HtmlPrintingTitleStr = cvar.HtmlPrintingTitleStr

class HtmlTag(_core.Object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlTag instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _html.HtmlTag_GetName(*args, **kwargs)

    def HasParam(*args, **kwargs):
        """HasParam(self, String par) -> bool"""
        return _html.HtmlTag_HasParam(*args, **kwargs)

    def GetParam(*args, **kwargs):
        """GetParam(self, String par, int with_commas=False) -> String"""
        return _html.HtmlTag_GetParam(*args, **kwargs)

    def GetAllParams(*args, **kwargs):
        """GetAllParams(self) -> String"""
        return _html.HtmlTag_GetAllParams(*args, **kwargs)

    def HasEnding(*args, **kwargs):
        """HasEnding(self) -> bool"""
        return _html.HtmlTag_HasEnding(*args, **kwargs)

    def GetBeginPos(*args, **kwargs):
        """GetBeginPos(self) -> int"""
        return _html.HtmlTag_GetBeginPos(*args, **kwargs)

    def GetEndPos1(*args, **kwargs):
        """GetEndPos1(self) -> int"""
        return _html.HtmlTag_GetEndPos1(*args, **kwargs)

    def GetEndPos2(*args, **kwargs):
        """GetEndPos2(self) -> int"""
        return _html.HtmlTag_GetEndPos2(*args, **kwargs)


class HtmlTagPtr(HtmlTag):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlTag
_html.HtmlTag_swigregister(HtmlTagPtr)

class HtmlParser(_core.Object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def SetFS(*args, **kwargs):
        """SetFS(self, FileSystem fs)"""
        return _html.HtmlParser_SetFS(*args, **kwargs)

    def GetFS(*args, **kwargs):
        """GetFS(self) -> FileSystem"""
        return _html.HtmlParser_GetFS(*args, **kwargs)

    def Parse(*args, **kwargs):
        """Parse(self, String source) -> Object"""
        return _html.HtmlParser_Parse(*args, **kwargs)

    def InitParser(*args, **kwargs):
        """InitParser(self, String source)"""
        return _html.HtmlParser_InitParser(*args, **kwargs)

    def DoneParser(*args, **kwargs):
        """DoneParser(self)"""
        return _html.HtmlParser_DoneParser(*args, **kwargs)

    def DoParsing(*args, **kwargs):
        """DoParsing(self, int begin_pos, int end_pos)"""
        return _html.HtmlParser_DoParsing(*args, **kwargs)

    def StopParsing(*args, **kwargs):
        """StopParsing(self)"""
        return _html.HtmlParser_StopParsing(*args, **kwargs)

    def AddTagHandler(*args, **kwargs):
        """AddTagHandler(self, HtmlTagHandler handler)"""
        return _html.HtmlParser_AddTagHandler(*args, **kwargs)

    def GetSource(*args, **kwargs):
        """GetSource(self) -> String"""
        return _html.HtmlParser_GetSource(*args, **kwargs)

    def PushTagHandler(*args, **kwargs):
        """PushTagHandler(self, HtmlTagHandler handler, String tags)"""
        return _html.HtmlParser_PushTagHandler(*args, **kwargs)

    def PopTagHandler(*args, **kwargs):
        """PopTagHandler(self)"""
        return _html.HtmlParser_PopTagHandler(*args, **kwargs)


class HtmlParserPtr(HtmlParser):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlParser
_html.HtmlParser_swigregister(HtmlParserPtr)

class HtmlWinParser(HtmlParser):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWinParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, HtmlWindow wnd=None) -> HtmlWinParser"""
        newobj = _html.new_HtmlWinParser(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetDC(*args, **kwargs):
        """SetDC(self, DC dc)"""
        return _html.HtmlWinParser_SetDC(*args, **kwargs)

    def GetDC(*args, **kwargs):
        """GetDC(self) -> DC"""
        return _html.HtmlWinParser_GetDC(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """GetCharHeight(self) -> int"""
        return _html.HtmlWinParser_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """GetCharWidth(self) -> int"""
        return _html.HtmlWinParser_GetCharWidth(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> HtmlWindow"""
        return _html.HtmlWinParser_GetWindow(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(self, String normal_face, String fixed_face, PyObject sizes=None)"""
        return _html.HtmlWinParser_SetFonts(*args, **kwargs)

    def SetStandardFonts(*args, **kwargs):
        """SetStandardFonts(self, int size=-1, String normal_face=EmptyString, String fixed_face=EmptyString)"""
        return _html.HtmlWinParser_SetStandardFonts(*args, **kwargs)

    def GetContainer(*args, **kwargs):
        """GetContainer(self) -> HtmlContainerCell"""
        return _html.HtmlWinParser_GetContainer(*args, **kwargs)

    def OpenContainer(*args, **kwargs):
        """OpenContainer(self) -> HtmlContainerCell"""
        return _html.HtmlWinParser_OpenContainer(*args, **kwargs)

    def SetContainer(*args, **kwargs):
        """SetContainer(self, HtmlContainerCell c) -> HtmlContainerCell"""
        return _html.HtmlWinParser_SetContainer(*args, **kwargs)

    def CloseContainer(*args, **kwargs):
        """CloseContainer(self) -> HtmlContainerCell"""
        return _html.HtmlWinParser_CloseContainer(*args, **kwargs)

    def GetFontSize(*args, **kwargs):
        """GetFontSize(self) -> int"""
        return _html.HtmlWinParser_GetFontSize(*args, **kwargs)

    def SetFontSize(*args, **kwargs):
        """SetFontSize(self, int s)"""
        return _html.HtmlWinParser_SetFontSize(*args, **kwargs)

    def GetFontBold(*args, **kwargs):
        """GetFontBold(self) -> int"""
        return _html.HtmlWinParser_GetFontBold(*args, **kwargs)

    def SetFontBold(*args, **kwargs):
        """SetFontBold(self, int x)"""
        return _html.HtmlWinParser_SetFontBold(*args, **kwargs)

    def GetFontItalic(*args, **kwargs):
        """GetFontItalic(self) -> int"""
        return _html.HtmlWinParser_GetFontItalic(*args, **kwargs)

    def SetFontItalic(*args, **kwargs):
        """SetFontItalic(self, int x)"""
        return _html.HtmlWinParser_SetFontItalic(*args, **kwargs)

    def GetFontUnderlined(*args, **kwargs):
        """GetFontUnderlined(self) -> int"""
        return _html.HtmlWinParser_GetFontUnderlined(*args, **kwargs)

    def SetFontUnderlined(*args, **kwargs):
        """SetFontUnderlined(self, int x)"""
        return _html.HtmlWinParser_SetFontUnderlined(*args, **kwargs)

    def GetFontFixed(*args, **kwargs):
        """GetFontFixed(self) -> int"""
        return _html.HtmlWinParser_GetFontFixed(*args, **kwargs)

    def SetFontFixed(*args, **kwargs):
        """SetFontFixed(self, int x)"""
        return _html.HtmlWinParser_SetFontFixed(*args, **kwargs)

    def GetAlign(*args, **kwargs):
        """GetAlign(self) -> int"""
        return _html.HtmlWinParser_GetAlign(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(self, int a)"""
        return _html.HtmlWinParser_SetAlign(*args, **kwargs)

    def GetLinkColor(*args, **kwargs):
        """GetLinkColor(self) -> Colour"""
        return _html.HtmlWinParser_GetLinkColor(*args, **kwargs)

    def SetLinkColor(*args, **kwargs):
        """SetLinkColor(self, Colour clr)"""
        return _html.HtmlWinParser_SetLinkColor(*args, **kwargs)

    def GetActualColor(*args, **kwargs):
        """GetActualColor(self) -> Colour"""
        return _html.HtmlWinParser_GetActualColor(*args, **kwargs)

    def SetActualColor(*args, **kwargs):
        """SetActualColor(self, Colour clr)"""
        return _html.HtmlWinParser_SetActualColor(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """SetLink(self, String link)"""
        return _html.HtmlWinParser_SetLink(*args, **kwargs)

    def CreateCurrentFont(*args, **kwargs):
        """CreateCurrentFont(self) -> Font"""
        return _html.HtmlWinParser_CreateCurrentFont(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """GetLink(self) -> HtmlLinkInfo"""
        return _html.HtmlWinParser_GetLink(*args, **kwargs)


class HtmlWinParserPtr(HtmlWinParser):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWinParser
_html.HtmlWinParser_swigregister(HtmlWinParserPtr)

class HtmlTagHandler(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlTagHandler"""
        newobj = _html.new_HtmlTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlTagHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _html.HtmlTagHandler__setCallbackInfo(*args, **kwargs)

    def SetParser(*args, **kwargs):
        """SetParser(self, HtmlParser parser)"""
        return _html.HtmlTagHandler_SetParser(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser(self) -> HtmlParser"""
        return _html.HtmlTagHandler_GetParser(*args, **kwargs)

    def ParseInner(*args, **kwargs):
        """ParseInner(self, HtmlTag tag)"""
        return _html.HtmlTagHandler_ParseInner(*args, **kwargs)


class HtmlTagHandlerPtr(HtmlTagHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlTagHandler
_html.HtmlTagHandler_swigregister(HtmlTagHandlerPtr)

class HtmlWinTagHandler(HtmlTagHandler):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlWinTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlWinTagHandler"""
        newobj = _html.new_HtmlWinTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWinTagHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _html.HtmlWinTagHandler__setCallbackInfo(*args, **kwargs)

    def SetParser(*args, **kwargs):
        """SetParser(self, HtmlParser parser)"""
        return _html.HtmlWinTagHandler_SetParser(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser(self) -> HtmlWinParser"""
        return _html.HtmlWinTagHandler_GetParser(*args, **kwargs)

    def ParseInner(*args, **kwargs):
        """ParseInner(self, HtmlTag tag)"""
        return _html.HtmlWinTagHandler_ParseInner(*args, **kwargs)


class HtmlWinTagHandlerPtr(HtmlWinTagHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWinTagHandler
_html.HtmlWinTagHandler_swigregister(HtmlWinTagHandlerPtr)


def HtmlWinParser_AddTagHandler(*args, **kwargs):
    """HtmlWinParser_AddTagHandler(PyObject tagHandlerClass)"""
    return _html.HtmlWinParser_AddTagHandler(*args, **kwargs)
#---------------------------------------------------------------------------

class HtmlSelection(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlSelection instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlSelection"""
        newobj = _html.new_HtmlSelection(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlSelection):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Set(*args, **kwargs):
        """Set(self, Point fromPos, HtmlCell fromCell, Point toPos, HtmlCell toCell)"""
        return _html.HtmlSelection_Set(*args, **kwargs)

    def SetCells(*args, **kwargs):
        """SetCells(self, HtmlCell fromCell, HtmlCell toCell)"""
        return _html.HtmlSelection_SetCells(*args, **kwargs)

    def GetFromCell(*args, **kwargs):
        """GetFromCell(self) -> HtmlCell"""
        return _html.HtmlSelection_GetFromCell(*args, **kwargs)

    def GetToCell(*args, **kwargs):
        """GetToCell(self) -> HtmlCell"""
        return _html.HtmlSelection_GetToCell(*args, **kwargs)

    def GetFromPos(*args, **kwargs):
        """GetFromPos(self) -> Point"""
        return _html.HtmlSelection_GetFromPos(*args, **kwargs)

    def GetToPos(*args, **kwargs):
        """GetToPos(self) -> Point"""
        return _html.HtmlSelection_GetToPos(*args, **kwargs)

    def GetFromPrivPos(*args, **kwargs):
        """GetFromPrivPos(self) -> Point"""
        return _html.HtmlSelection_GetFromPrivPos(*args, **kwargs)

    def GetToPrivPos(*args, **kwargs):
        """GetToPrivPos(self) -> Point"""
        return _html.HtmlSelection_GetToPrivPos(*args, **kwargs)

    def SetFromPrivPos(*args, **kwargs):
        """SetFromPrivPos(self, Point pos)"""
        return _html.HtmlSelection_SetFromPrivPos(*args, **kwargs)

    def SetToPrivPos(*args, **kwargs):
        """SetToPrivPos(self, Point pos)"""
        return _html.HtmlSelection_SetToPrivPos(*args, **kwargs)

    def ClearPrivPos(*args, **kwargs):
        """ClearPrivPos(self)"""
        return _html.HtmlSelection_ClearPrivPos(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return _html.HtmlSelection_IsEmpty(*args, **kwargs)


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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingState instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlRenderingState"""
        newobj = _html.new_HtmlRenderingState(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingState):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelectionState(*args, **kwargs):
        """SetSelectionState(self, int s)"""
        return _html.HtmlRenderingState_SetSelectionState(*args, **kwargs)

    def GetSelectionState(*args, **kwargs):
        """GetSelectionState(self) -> int"""
        return _html.HtmlRenderingState_GetSelectionState(*args, **kwargs)

    def SetFgColour(*args, **kwargs):
        """SetFgColour(self, Colour c)"""
        return _html.HtmlRenderingState_SetFgColour(*args, **kwargs)

    def GetFgColour(*args, **kwargs):
        """GetFgColour(self) -> Colour"""
        return _html.HtmlRenderingState_GetFgColour(*args, **kwargs)

    def SetBgColour(*args, **kwargs):
        """SetBgColour(self, Colour c)"""
        return _html.HtmlRenderingState_SetBgColour(*args, **kwargs)

    def GetBgColour(*args, **kwargs):
        """GetBgColour(self) -> Colour"""
        return _html.HtmlRenderingState_GetBgColour(*args, **kwargs)


class HtmlRenderingStatePtr(HtmlRenderingState):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlRenderingState
_html.HtmlRenderingState_swigregister(HtmlRenderingStatePtr)

class HtmlRenderingStyle(object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingStyle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetSelectedTextColour(*args, **kwargs):
        """GetSelectedTextColour(self, Colour clr) -> Colour"""
        return _html.HtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)

    def GetSelectedTextBgColour(*args, **kwargs):
        """GetSelectedTextBgColour(self, Colour clr) -> Colour"""
        return _html.HtmlRenderingStyle_GetSelectedTextBgColour(*args, **kwargs)


class HtmlRenderingStylePtr(HtmlRenderingStyle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlRenderingStyle
_html.HtmlRenderingStyle_swigregister(HtmlRenderingStylePtr)

class DefaultHtmlRenderingStyle(HtmlRenderingStyle):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDefaultHtmlRenderingStyle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetSelectedTextColour(*args, **kwargs):
        """GetSelectedTextColour(self, Colour clr) -> Colour"""
        return _html.DefaultHtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)

    def GetSelectedTextBgColour(*args, **kwargs):
        """GetSelectedTextBgColour(self, Colour clr) -> Colour"""
        return _html.DefaultHtmlRenderingStyle_GetSelectedTextBgColour(*args, **kwargs)


class DefaultHtmlRenderingStylePtr(DefaultHtmlRenderingStyle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DefaultHtmlRenderingStyle
_html.DefaultHtmlRenderingStyle_swigregister(DefaultHtmlRenderingStylePtr)

class HtmlRenderingInfo(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlRenderingInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlRenderingInfo"""
        newobj = _html.new_HtmlRenderingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingInfo):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelection(*args, **kwargs):
        """SetSelection(self, HtmlSelection s)"""
        return _html.HtmlRenderingInfo_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> HtmlSelection"""
        return _html.HtmlRenderingInfo_GetSelection(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, HtmlRenderingStyle style)"""
        return _html.HtmlRenderingInfo_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> HtmlRenderingStyle"""
        return _html.HtmlRenderingInfo_GetStyle(*args, **kwargs)

    def GetState(*args, **kwargs):
        """GetState(self) -> HtmlRenderingState"""
        return _html.HtmlRenderingInfo_GetState(*args, **kwargs)


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
class HtmlCell(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlCell"""
        newobj = _html.new_HtmlCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosX(*args, **kwargs):
        """GetPosX(self) -> int"""
        return _html.HtmlCell_GetPosX(*args, **kwargs)

    def GetPosY(*args, **kwargs):
        """GetPosY(self) -> int"""
        return _html.HtmlCell_GetPosY(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _html.HtmlCell_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _html.HtmlCell_GetHeight(*args, **kwargs)

    def GetDescent(*args, **kwargs):
        """GetDescent(self) -> int"""
        return _html.HtmlCell_GetDescent(*args, **kwargs)

    def GetMaxTotalWidth(*args, **kwargs):
        """GetMaxTotalWidth(self) -> int"""
        return _html.HtmlCell_GetMaxTotalWidth(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> String"""
        return _html.HtmlCell_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, String id)"""
        return _html.HtmlCell_SetId(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """GetLink(self, int x=0, int y=0) -> HtmlLinkInfo"""
        return _html.HtmlCell_GetLink(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext(self) -> HtmlCell"""
        return _html.HtmlCell_GetNext(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> HtmlContainerCell"""
        return _html.HtmlCell_GetParent(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild(self) -> HtmlCell"""
        return _html.HtmlCell_GetFirstChild(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """GetCursor(self) -> Cursor"""
        return _html.HtmlCell_GetCursor(*args, **kwargs)

    def IsFormattingCell(*args, **kwargs):
        """IsFormattingCell(self) -> bool"""
        return _html.HtmlCell_IsFormattingCell(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """SetLink(self, HtmlLinkInfo link)"""
        return _html.HtmlCell_SetLink(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(self, HtmlCell cell)"""
        return _html.HtmlCell_SetNext(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(self, HtmlContainerCell p)"""
        return _html.HtmlCell_SetParent(*args, **kwargs)

    def SetPos(*args, **kwargs):
        """SetPos(self, int x, int y)"""
        return _html.HtmlCell_SetPos(*args, **kwargs)

    def Layout(*args, **kwargs):
        """Layout(self, int w)"""
        return _html.HtmlCell_Layout(*args, **kwargs)

    def Draw(*args, **kwargs):
        """Draw(self, DC dc, int x, int y, int view_y1, int view_y2, HtmlRenderingInfo info)"""
        return _html.HtmlCell_Draw(*args, **kwargs)

    def DrawInvisible(*args, **kwargs):
        """DrawInvisible(self, DC dc, int x, int y, HtmlRenderingInfo info)"""
        return _html.HtmlCell_DrawInvisible(*args, **kwargs)

    def Find(*args, **kwargs):
        """Find(self, int condition, void param) -> HtmlCell"""
        return _html.HtmlCell_Find(*args, **kwargs)

    def AdjustPagebreak(*args, **kwargs):
        """AdjustPagebreak(self, int INOUT) -> bool"""
        return _html.HtmlCell_AdjustPagebreak(*args, **kwargs)

    def SetCanLiveOnPagebreak(*args, **kwargs):
        """SetCanLiveOnPagebreak(self, bool can)"""
        return _html.HtmlCell_SetCanLiveOnPagebreak(*args, **kwargs)

    def IsLinebreakAllowed(*args, **kwargs):
        """IsLinebreakAllowed(self) -> bool"""
        return _html.HtmlCell_IsLinebreakAllowed(*args, **kwargs)

    def IsTerminalCell(*args, **kwargs):
        """IsTerminalCell(self) -> bool"""
        return _html.HtmlCell_IsTerminalCell(*args, **kwargs)

    def FindCellByPos(*args, **kwargs):
        """FindCellByPos(self, int x, int y, unsigned int flags=HTML_FIND_EXACT) -> HtmlCell"""
        return _html.HtmlCell_FindCellByPos(*args, **kwargs)

    def GetAbsPos(*args, **kwargs):
        """GetAbsPos(self) -> Point"""
        return _html.HtmlCell_GetAbsPos(*args, **kwargs)

    def GetFirstTerminal(*args, **kwargs):
        """GetFirstTerminal(self) -> HtmlCell"""
        return _html.HtmlCell_GetFirstTerminal(*args, **kwargs)

    def GetLastTerminal(*args, **kwargs):
        """GetLastTerminal(self) -> HtmlCell"""
        return _html.HtmlCell_GetLastTerminal(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth(self) -> unsigned int"""
        return _html.HtmlCell_GetDepth(*args, **kwargs)

    def IsBefore(*args, **kwargs):
        """IsBefore(self, HtmlCell cell) -> bool"""
        return _html.HtmlCell_IsBefore(*args, **kwargs)

    def ConvertToText(*args, **kwargs):
        """ConvertToText(self, HtmlSelection sel) -> String"""
        return _html.HtmlCell_ConvertToText(*args, **kwargs)


class HtmlCellPtr(HtmlCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlCell
_html.HtmlCell_swigregister(HtmlCellPtr)

class HtmlWordCell(HtmlCell):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWordCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String word, DC dc) -> HtmlWordCell"""
        newobj = _html.new_HtmlWordCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class HtmlWordCellPtr(HtmlWordCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWordCell
_html.HtmlWordCell_swigregister(HtmlWordCellPtr)

class HtmlContainerCell(HtmlCell):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlContainerCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, HtmlContainerCell parent) -> HtmlContainerCell"""
        newobj = _html.new_HtmlContainerCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def InsertCell(*args, **kwargs):
        """InsertCell(self, HtmlCell cell)"""
        return _html.HtmlContainerCell_InsertCell(*args, **kwargs)

    def SetAlignHor(*args, **kwargs):
        """SetAlignHor(self, int al)"""
        return _html.HtmlContainerCell_SetAlignHor(*args, **kwargs)

    def GetAlignHor(*args, **kwargs):
        """GetAlignHor(self) -> int"""
        return _html.HtmlContainerCell_GetAlignHor(*args, **kwargs)

    def SetAlignVer(*args, **kwargs):
        """SetAlignVer(self, int al)"""
        return _html.HtmlContainerCell_SetAlignVer(*args, **kwargs)

    def GetAlignVer(*args, **kwargs):
        """GetAlignVer(self) -> int"""
        return _html.HtmlContainerCell_GetAlignVer(*args, **kwargs)

    def SetIndent(*args, **kwargs):
        """SetIndent(self, int i, int what, int units=HTML_UNITS_PIXELS)"""
        return _html.HtmlContainerCell_SetIndent(*args, **kwargs)

    def GetIndent(*args, **kwargs):
        """GetIndent(self, int ind) -> int"""
        return _html.HtmlContainerCell_GetIndent(*args, **kwargs)

    def GetIndentUnits(*args, **kwargs):
        """GetIndentUnits(self, int ind) -> int"""
        return _html.HtmlContainerCell_GetIndentUnits(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(self, HtmlTag tag)"""
        return _html.HtmlContainerCell_SetAlign(*args, **kwargs)

    def SetWidthFloat(*args, **kwargs):
        """SetWidthFloat(self, int w, int units)"""
        return _html.HtmlContainerCell_SetWidthFloat(*args, **kwargs)

    def SetWidthFloatFromTag(*args, **kwargs):
        """SetWidthFloatFromTag(self, HtmlTag tag)"""
        return _html.HtmlContainerCell_SetWidthFloatFromTag(*args, **kwargs)

    def SetMinHeight(*args, **kwargs):
        """SetMinHeight(self, int h, int align=HTML_ALIGN_TOP)"""
        return _html.HtmlContainerCell_SetMinHeight(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour clr)"""
        return _html.HtmlContainerCell_SetBackgroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _html.HtmlContainerCell_GetBackgroundColour(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """SetBorder(self, Colour clr1, Colour clr2)"""
        return _html.HtmlContainerCell_SetBorder(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild(self) -> HtmlCell"""
        return _html.HtmlContainerCell_GetFirstChild(*args, **kwargs)


class HtmlContainerCellPtr(HtmlContainerCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlContainerCell
_html.HtmlContainerCell_swigregister(HtmlContainerCellPtr)

class HtmlColourCell(HtmlCell):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlColourCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Colour clr, int flags=HTML_CLR_FOREGROUND) -> HtmlColourCell"""
        newobj = _html.new_HtmlColourCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class HtmlColourCellPtr(HtmlColourCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlColourCell
_html.HtmlColourCell_swigregister(HtmlColourCellPtr)

class HtmlFontCell(HtmlCell):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlFontCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Font font) -> HtmlFontCell"""
        newobj = _html.new_HtmlFontCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class HtmlFontCellPtr(HtmlFontCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlFontCell
_html.HtmlFontCell_swigregister(HtmlFontCellPtr)

class HtmlWidgetCell(HtmlCell):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlWidgetCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Window wnd, int w=0) -> HtmlWidgetCell"""
        newobj = _html.new_HtmlWidgetCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class HtmlWidgetCellPtr(HtmlWidgetCell):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWidgetCell
_html.HtmlWidgetCell_swigregister(HtmlWidgetCellPtr)

#---------------------------------------------------------------------------

class HtmlFilter(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlFilter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlFilter"""
        newobj = _html.new_HtmlFilter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlFilter)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _html.HtmlFilter__setCallbackInfo(*args, **kwargs)


class HtmlFilterPtr(HtmlFilter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlFilter
_html.HtmlFilter_swigregister(HtmlFilterPtr)

#---------------------------------------------------------------------------

class HtmlWindow(_windows.ScrolledWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, int style=HW_DEFAULT_STYLE, 
            String name=HtmlWindowNameStr) -> HtmlWindow
        """
        newobj = _html.new_HtmlWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWindow); self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, int style=HW_SCROLLBAR_AUTO, 
            String name=HtmlWindowNameStr) -> bool
        """
        return _html.HtmlWindow_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _html.HtmlWindow__setCallbackInfo(*args, **kwargs)

    def SetPage(*args, **kwargs):
        """SetPage(self, String source) -> bool"""
        return _html.HtmlWindow_SetPage(*args, **kwargs)

    def LoadPage(*args, **kwargs):
        """LoadPage(self, String location) -> bool"""
        return _html.HtmlWindow_LoadPage(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename) -> bool"""
        return _html.HtmlWindow_LoadFile(*args, **kwargs)

    def AppendToPage(*args, **kwargs):
        """AppendToPage(self, String source) -> bool"""
        return _html.HtmlWindow_AppendToPage(*args, **kwargs)

    def GetOpenedPage(*args, **kwargs):
        """GetOpenedPage(self) -> String"""
        return _html.HtmlWindow_GetOpenedPage(*args, **kwargs)

    def GetOpenedAnchor(*args, **kwargs):
        """GetOpenedAnchor(self) -> String"""
        return _html.HtmlWindow_GetOpenedAnchor(*args, **kwargs)

    def GetOpenedPageTitle(*args, **kwargs):
        """GetOpenedPageTitle(self) -> String"""
        return _html.HtmlWindow_GetOpenedPageTitle(*args, **kwargs)

    def SetRelatedFrame(*args, **kwargs):
        """SetRelatedFrame(self, Frame frame, String format)"""
        return _html.HtmlWindow_SetRelatedFrame(*args, **kwargs)

    def GetRelatedFrame(*args, **kwargs):
        """GetRelatedFrame(self) -> Frame"""
        return _html.HtmlWindow_GetRelatedFrame(*args, **kwargs)

    def SetRelatedStatusBar(*args, **kwargs):
        """SetRelatedStatusBar(self, int bar)"""
        return _html.HtmlWindow_SetRelatedStatusBar(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(self, String normal_face, String fixed_face, PyObject sizes=None)"""
        return _html.HtmlWindow_SetFonts(*args, **kwargs)

    def SetStandardFonts(*args, **kwargs):
        """SetStandardFonts(self, int size=-1, String normal_face=EmptyString, String fixed_face=EmptyString)"""
        return _html.HtmlWindow_SetStandardFonts(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(self, String title)"""
        return _html.HtmlWindow_SetTitle(*args, **kwargs)

    def SetBorders(*args, **kwargs):
        """SetBorders(self, int b)"""
        return _html.HtmlWindow_SetBorders(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlWindow_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlWindow_WriteCustomization(*args, **kwargs)

    def HistoryBack(*args, **kwargs):
        """HistoryBack(self) -> bool"""
        return _html.HtmlWindow_HistoryBack(*args, **kwargs)

    def HistoryForward(*args, **kwargs):
        """HistoryForward(self) -> bool"""
        return _html.HtmlWindow_HistoryForward(*args, **kwargs)

    def HistoryCanBack(*args, **kwargs):
        """HistoryCanBack(self) -> bool"""
        return _html.HtmlWindow_HistoryCanBack(*args, **kwargs)

    def HistoryCanForward(*args, **kwargs):
        """HistoryCanForward(self) -> bool"""
        return _html.HtmlWindow_HistoryCanForward(*args, **kwargs)

    def HistoryClear(*args, **kwargs):
        """HistoryClear(self)"""
        return _html.HtmlWindow_HistoryClear(*args, **kwargs)

    def GetInternalRepresentation(*args, **kwargs):
        """GetInternalRepresentation(self) -> HtmlContainerCell"""
        return _html.HtmlWindow_GetInternalRepresentation(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser(self) -> HtmlWinParser"""
        return _html.HtmlWindow_GetParser(*args, **kwargs)

    def ScrollToAnchor(*args, **kwargs):
        """ScrollToAnchor(self, String anchor) -> bool"""
        return _html.HtmlWindow_ScrollToAnchor(*args, **kwargs)

    def HasAnchor(*args, **kwargs):
        """HasAnchor(self, String anchor) -> bool"""
        return _html.HtmlWindow_HasAnchor(*args, **kwargs)

    def AddFilter(*args, **kwargs):
        """AddFilter(HtmlFilter filter)"""
        return _html.HtmlWindow_AddFilter(*args, **kwargs)

    AddFilter = staticmethod(AddFilter)
    def SelectWord(*args, **kwargs):
        """SelectWord(self, Point pos)"""
        return _html.HtmlWindow_SelectWord(*args, **kwargs)

    def SelectLine(*args, **kwargs):
        """SelectLine(self, Point pos)"""
        return _html.HtmlWindow_SelectLine(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll(self)"""
        return _html.HtmlWindow_SelectAll(*args, **kwargs)

    def SelectionToText(*args, **kwargs):
        """SelectionToText(self) -> String"""
        return _html.HtmlWindow_SelectionToText(*args, **kwargs)

    def ToText(*args, **kwargs):
        """ToText(self) -> String"""
        return _html.HtmlWindow_ToText(*args, **kwargs)

    def base_OnLinkClicked(*args, **kwargs):
        """base_OnLinkClicked(self, HtmlLinkInfo link)"""
        return _html.HtmlWindow_base_OnLinkClicked(*args, **kwargs)

    def base_OnSetTitle(*args, **kwargs):
        """base_OnSetTitle(self, String title)"""
        return _html.HtmlWindow_base_OnSetTitle(*args, **kwargs)

    def base_OnCellMouseHover(*args, **kwargs):
        """base_OnCellMouseHover(self, HtmlCell cell, int x, int y)"""
        return _html.HtmlWindow_base_OnCellMouseHover(*args, **kwargs)

    def base_OnCellClicked(*args, **kwargs):
        """base_OnCellClicked(self, HtmlCell cell, int x, int y, MouseEvent event)"""
        return _html.HtmlWindow_base_OnCellClicked(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _html.HtmlWindow_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)

class HtmlWindowPtr(HtmlWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWindow
_html.HtmlWindow_swigregister(HtmlWindowPtr)

def PreHtmlWindow(*args, **kwargs):
    """PreHtmlWindow() -> HtmlWindow"""
    val = _html.new_PreHtmlWindow(*args, **kwargs)
    val.thisown = 1
    return val

def HtmlWindow_AddFilter(*args, **kwargs):
    """HtmlWindow_AddFilter(HtmlFilter filter)"""
    return _html.HtmlWindow_AddFilter(*args, **kwargs)

def HtmlWindow_GetClassDefaultAttributes(*args, **kwargs):
    """
    HtmlWindow_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
    return _html.HtmlWindow_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class HtmlDCRenderer(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlDCRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlDCRenderer"""
        newobj = _html.new_HtmlDCRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlDCRenderer):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetDC(*args, **kwargs):
        """SetDC(self, DC dc, int maxwidth)"""
        return _html.HtmlDCRenderer_SetDC(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, int width, int height)"""
        return _html.HtmlDCRenderer_SetSize(*args, **kwargs)

    def SetHtmlText(*args, **kwargs):
        """SetHtmlText(self, String html, String basepath=EmptyString, bool isdir=True)"""
        return _html.HtmlDCRenderer_SetHtmlText(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(self, String normal_face, String fixed_face, PyObject sizes=None)"""
        return _html.HtmlDCRenderer_SetFonts(*args, **kwargs)

    def SetStandardFonts(*args, **kwargs):
        """SetStandardFonts(self, int size=-1, String normal_face=EmptyString, String fixed_face=EmptyString)"""
        return _html.HtmlDCRenderer_SetStandardFonts(*args, **kwargs)

    def Render(*args, **kwargs):
        """
        Render(self, int x, int y, int from=0, int dont_render=False, int to=INT_MAX, 
            int choices=None, int LCOUNT=0) -> int
        """
        return _html.HtmlDCRenderer_Render(*args, **kwargs)

    def GetTotalHeight(*args, **kwargs):
        """GetTotalHeight(self) -> int"""
        return _html.HtmlDCRenderer_GetTotalHeight(*args, **kwargs)


class HtmlDCRendererPtr(HtmlDCRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlDCRenderer
_html.HtmlDCRenderer_swigregister(HtmlDCRendererPtr)

PAGE_ODD = _html.PAGE_ODD
PAGE_EVEN = _html.PAGE_EVEN
PAGE_ALL = _html.PAGE_ALL
class HtmlPrintout(_windows.Printout):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlPrintout instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String title=HtmlPrintoutTitleStr) -> HtmlPrintout"""
        newobj = _html.new_HtmlPrintout(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetHtmlText(*args, **kwargs):
        """SetHtmlText(self, String html, String basepath=EmptyString, bool isdir=True)"""
        return _html.HtmlPrintout_SetHtmlText(*args, **kwargs)

    def SetHtmlFile(*args, **kwargs):
        """SetHtmlFile(self, String htmlfile)"""
        return _html.HtmlPrintout_SetHtmlFile(*args, **kwargs)

    def SetHeader(*args, **kwargs):
        """SetHeader(self, String header, int pg=PAGE_ALL)"""
        return _html.HtmlPrintout_SetHeader(*args, **kwargs)

    def SetFooter(*args, **kwargs):
        """SetFooter(self, String footer, int pg=PAGE_ALL)"""
        return _html.HtmlPrintout_SetFooter(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(self, String normal_face, String fixed_face, PyObject sizes=None)"""
        return _html.HtmlPrintout_SetFonts(*args, **kwargs)

    def SetStandardFonts(*args, **kwargs):
        """SetStandardFonts(self, int size=-1, String normal_face=EmptyString, String fixed_face=EmptyString)"""
        return _html.HtmlPrintout_SetStandardFonts(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """
        SetMargins(self, float top=25.2, float bottom=25.2, float left=25.2, 
            float right=25.2, float spaces=5)
        """
        return _html.HtmlPrintout_SetMargins(*args, **kwargs)

    def AddFilter(*args, **kwargs):
        """AddFilter(wxHtmlFilter filter)"""
        return _html.HtmlPrintout_AddFilter(*args, **kwargs)

    AddFilter = staticmethod(AddFilter)
    def CleanUpStatics(*args, **kwargs):
        """CleanUpStatics()"""
        return _html.HtmlPrintout_CleanUpStatics(*args, **kwargs)

    CleanUpStatics = staticmethod(CleanUpStatics)

class HtmlPrintoutPtr(HtmlPrintout):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlPrintout
_html.HtmlPrintout_swigregister(HtmlPrintoutPtr)

def HtmlPrintout_AddFilter(*args, **kwargs):
    """HtmlPrintout_AddFilter(wxHtmlFilter filter)"""
    return _html.HtmlPrintout_AddFilter(*args, **kwargs)

def HtmlPrintout_CleanUpStatics(*args, **kwargs):
    """HtmlPrintout_CleanUpStatics()"""
    return _html.HtmlPrintout_CleanUpStatics(*args, **kwargs)

class HtmlEasyPrinting(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlEasyPrinting instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String name=HtmlPrintingTitleStr, Window parentWindow=None) -> HtmlEasyPrinting"""
        newobj = _html.new_HtmlEasyPrinting(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlEasyPrinting):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def PreviewFile(*args, **kwargs):
        """PreviewFile(self, String htmlfile)"""
        return _html.HtmlEasyPrinting_PreviewFile(*args, **kwargs)

    def PreviewText(*args, **kwargs):
        """PreviewText(self, String htmltext, String basepath=EmptyString)"""
        return _html.HtmlEasyPrinting_PreviewText(*args, **kwargs)

    def PrintFile(*args, **kwargs):
        """PrintFile(self, String htmlfile)"""
        return _html.HtmlEasyPrinting_PrintFile(*args, **kwargs)

    def PrintText(*args, **kwargs):
        """PrintText(self, String htmltext, String basepath=EmptyString)"""
        return _html.HtmlEasyPrinting_PrintText(*args, **kwargs)

    def PageSetup(*args, **kwargs):
        """PageSetup(self)"""
        return _html.HtmlEasyPrinting_PageSetup(*args, **kwargs)

    def SetHeader(*args, **kwargs):
        """SetHeader(self, String header, int pg=PAGE_ALL)"""
        return _html.HtmlEasyPrinting_SetHeader(*args, **kwargs)

    def SetFooter(*args, **kwargs):
        """SetFooter(self, String footer, int pg=PAGE_ALL)"""
        return _html.HtmlEasyPrinting_SetFooter(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(self, String normal_face, String fixed_face, PyObject sizes=None)"""
        return _html.HtmlEasyPrinting_SetFonts(*args, **kwargs)

    def SetStandardFonts(*args, **kwargs):
        """SetStandardFonts(self, int size=-1, String normal_face=EmptyString, String fixed_face=EmptyString)"""
        return _html.HtmlEasyPrinting_SetStandardFonts(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData(self) -> PrintData"""
        return _html.HtmlEasyPrinting_GetPrintData(*args, **kwargs)

    def GetPageSetupData(*args, **kwargs):
        """GetPageSetupData(self) -> PageSetupDialogData"""
        return _html.HtmlEasyPrinting_GetPageSetupData(*args, **kwargs)


class HtmlEasyPrintingPtr(HtmlEasyPrinting):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlEasyPrinting
_html.HtmlEasyPrinting_swigregister(HtmlEasyPrintingPtr)

#---------------------------------------------------------------------------

class HtmlBookRecord(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlBookRecord instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String bookfile, String basepath, String title, String start) -> HtmlBookRecord"""
        newobj = _html.new_HtmlBookRecord(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetBookFile(*args, **kwargs):
        """GetBookFile(self) -> String"""
        return _html.HtmlBookRecord_GetBookFile(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle(self) -> String"""
        return _html.HtmlBookRecord_GetTitle(*args, **kwargs)

    def GetStart(*args, **kwargs):
        """GetStart(self) -> String"""
        return _html.HtmlBookRecord_GetStart(*args, **kwargs)

    def GetBasePath(*args, **kwargs):
        """GetBasePath(self) -> String"""
        return _html.HtmlBookRecord_GetBasePath(*args, **kwargs)

    def SetContentsRange(*args, **kwargs):
        """SetContentsRange(self, int start, int end)"""
        return _html.HtmlBookRecord_SetContentsRange(*args, **kwargs)

    def GetContentsStart(*args, **kwargs):
        """GetContentsStart(self) -> int"""
        return _html.HtmlBookRecord_GetContentsStart(*args, **kwargs)

    def GetContentsEnd(*args, **kwargs):
        """GetContentsEnd(self) -> int"""
        return _html.HtmlBookRecord_GetContentsEnd(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(self, String title)"""
        return _html.HtmlBookRecord_SetTitle(*args, **kwargs)

    def SetBasePath(*args, **kwargs):
        """SetBasePath(self, String path)"""
        return _html.HtmlBookRecord_SetBasePath(*args, **kwargs)

    def SetStart(*args, **kwargs):
        """SetStart(self, String start)"""
        return _html.HtmlBookRecord_SetStart(*args, **kwargs)

    def GetFullPath(*args, **kwargs):
        """GetFullPath(self, String page) -> String"""
        return _html.HtmlBookRecord_GetFullPath(*args, **kwargs)


class HtmlBookRecordPtr(HtmlBookRecord):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlBookRecord
_html.HtmlBookRecord_swigregister(HtmlBookRecordPtr)

class HtmlContentsItem(object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlContentsItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetLevel(*args, **kwargs):
        """GetLevel(self) -> int"""
        return _html.HtmlContentsItem_GetLevel(*args, **kwargs)

    def GetID(*args, **kwargs):
        """GetID(self) -> int"""
        return _html.HtmlContentsItem_GetID(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _html.HtmlContentsItem_GetName(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self) -> String"""
        return _html.HtmlContentsItem_GetPage(*args, **kwargs)

    def GetBook(*args, **kwargs):
        """GetBook(self) -> HtmlBookRecord"""
        return _html.HtmlContentsItem_GetBook(*args, **kwargs)


class HtmlContentsItemPtr(HtmlContentsItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlContentsItem
_html.HtmlContentsItem_swigregister(HtmlContentsItemPtr)

class HtmlSearchStatus(object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlSearchStatus instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Search(*args, **kwargs):
        """Search(self) -> bool"""
        return _html.HtmlSearchStatus_Search(*args, **kwargs)

    def IsActive(*args, **kwargs):
        """IsActive(self) -> bool"""
        return _html.HtmlSearchStatus_IsActive(*args, **kwargs)

    def GetCurIndex(*args, **kwargs):
        """GetCurIndex(self) -> int"""
        return _html.HtmlSearchStatus_GetCurIndex(*args, **kwargs)

    def GetMaxIndex(*args, **kwargs):
        """GetMaxIndex(self) -> int"""
        return _html.HtmlSearchStatus_GetMaxIndex(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _html.HtmlSearchStatus_GetName(*args, **kwargs)

    def GetContentsItem(*args, **kwargs):
        """GetContentsItem(self) -> HtmlContentsItem"""
        return _html.HtmlSearchStatus_GetContentsItem(*args, **kwargs)


class HtmlSearchStatusPtr(HtmlSearchStatus):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlSearchStatus
_html.HtmlSearchStatus_swigregister(HtmlSearchStatusPtr)

class HtmlHelpData(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> HtmlHelpData"""
        newobj = _html.new_HtmlHelpData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlHelpData):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetTempDir(*args, **kwargs):
        """SetTempDir(self, String path)"""
        return _html.HtmlHelpData_SetTempDir(*args, **kwargs)

    def AddBook(*args, **kwargs):
        """AddBook(self, String book) -> bool"""
        return _html.HtmlHelpData_AddBook(*args, **kwargs)

    def FindPageByName(*args, **kwargs):
        """FindPageByName(self, String page) -> String"""
        return _html.HtmlHelpData_FindPageByName(*args, **kwargs)

    def FindPageById(*args, **kwargs):
        """FindPageById(self, int id) -> String"""
        return _html.HtmlHelpData_FindPageById(*args, **kwargs)

    def GetBookRecArray(*args, **kwargs):
        """GetBookRecArray(self) -> wxHtmlBookRecArray"""
        return _html.HtmlHelpData_GetBookRecArray(*args, **kwargs)

    def GetContents(*args, **kwargs):
        """GetContents(self) -> HtmlContentsItem"""
        return _html.HtmlHelpData_GetContents(*args, **kwargs)

    def GetContentsCnt(*args, **kwargs):
        """GetContentsCnt(self) -> int"""
        return _html.HtmlHelpData_GetContentsCnt(*args, **kwargs)

    def GetIndex(*args, **kwargs):
        """GetIndex(self) -> HtmlContentsItem"""
        return _html.HtmlHelpData_GetIndex(*args, **kwargs)

    def GetIndexCnt(*args, **kwargs):
        """GetIndexCnt(self) -> int"""
        return _html.HtmlHelpData_GetIndexCnt(*args, **kwargs)


class HtmlHelpDataPtr(HtmlHelpData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpData
_html.HtmlHelpData_swigregister(HtmlHelpDataPtr)

class HtmlHelpFrame(_windows.Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int ??, String title=EmptyString, int style=HF_DEFAULTSTYLE, 
            HtmlHelpData data=None) -> HtmlHelpFrame
        """
        newobj = _html.new_HtmlHelpFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetData(*args, **kwargs):
        """GetData(self) -> HtmlHelpData"""
        return _html.HtmlHelpFrame_GetData(*args, **kwargs)

    def SetTitleFormat(*args, **kwargs):
        """SetTitleFormat(self, String format)"""
        return _html.HtmlHelpFrame_SetTitleFormat(*args, **kwargs)

    def Display(*args, **kwargs):
        """Display(self, String x)"""
        return _html.HtmlHelpFrame_Display(*args, **kwargs)

    def DisplayID(*args, **kwargs):
        """DisplayID(self, int id)"""
        return _html.HtmlHelpFrame_DisplayID(*args, **kwargs)

    def DisplayContents(*args, **kwargs):
        """DisplayContents(self)"""
        return _html.HtmlHelpFrame_DisplayContents(*args, **kwargs)

    def DisplayIndex(*args, **kwargs):
        """DisplayIndex(self)"""
        return _html.HtmlHelpFrame_DisplayIndex(*args, **kwargs)

    def KeywordSearch(*args, **kwargs):
        """KeywordSearch(self, String keyword) -> bool"""
        return _html.HtmlHelpFrame_KeywordSearch(*args, **kwargs)

    def UseConfig(*args, **kwargs):
        """UseConfig(self, ConfigBase config, String rootpath=EmptyString)"""
        return _html.HtmlHelpFrame_UseConfig(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlHelpFrame_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlHelpFrame_WriteCustomization(*args, **kwargs)


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
class HtmlHelpController(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpController instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int style=HF_DEFAULTSTYLE) -> HtmlHelpController"""
        newobj = _html.new_HtmlHelpController(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlHelpController):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetTitleFormat(*args, **kwargs):
        """SetTitleFormat(self, String format)"""
        return _html.HtmlHelpController_SetTitleFormat(*args, **kwargs)

    def SetTempDir(*args, **kwargs):
        """SetTempDir(self, String path)"""
        return _html.HtmlHelpController_SetTempDir(*args, **kwargs)

    def AddBook(*args, **kwargs):
        """AddBook(self, String book, int show_wait_msg=False) -> bool"""
        return _html.HtmlHelpController_AddBook(*args, **kwargs)

    def Display(*args, **kwargs):
        """Display(self, String x)"""
        return _html.HtmlHelpController_Display(*args, **kwargs)

    def DisplayID(*args, **kwargs):
        """DisplayID(self, int id)"""
        return _html.HtmlHelpController_DisplayID(*args, **kwargs)

    def DisplayContents(*args, **kwargs):
        """DisplayContents(self)"""
        return _html.HtmlHelpController_DisplayContents(*args, **kwargs)

    def DisplayIndex(*args, **kwargs):
        """DisplayIndex(self)"""
        return _html.HtmlHelpController_DisplayIndex(*args, **kwargs)

    def KeywordSearch(*args, **kwargs):
        """KeywordSearch(self, String keyword) -> bool"""
        return _html.HtmlHelpController_KeywordSearch(*args, **kwargs)

    def UseConfig(*args, **kwargs):
        """UseConfig(self, ConfigBase config, String rootpath=EmptyString)"""
        return _html.HtmlHelpController_UseConfig(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlHelpController_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(self, ConfigBase cfg, String path=EmptyString)"""
        return _html.HtmlHelpController_WriteCustomization(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self) -> HtmlHelpFrame"""
        return _html.HtmlHelpController_GetFrame(*args, **kwargs)


class HtmlHelpControllerPtr(HtmlHelpController):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpController
_html.HtmlHelpController_swigregister(HtmlHelpControllerPtr)


