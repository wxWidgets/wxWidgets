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
class HtmlLinkInfo(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlLinkInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString href, wxString target=wxPyEmptyString) -> HtmlLinkInfo"""
        newobj = _html.new_HtmlLinkInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHref(*args, **kwargs):
        """GetHref() -> wxString"""
        return _html.HtmlLinkInfo_GetHref(*args, **kwargs)

    def GetTarget(*args, **kwargs):
        """GetTarget() -> wxString"""
        return _html.HtmlLinkInfo_GetTarget(*args, **kwargs)

    def GetEvent(*args, **kwargs):
        """GetEvent() -> MouseEvent"""
        return _html.HtmlLinkInfo_GetEvent(*args, **kwargs)

    def GetHtmlCell(*args, **kwargs):
        """GetHtmlCell() -> HtmlCell"""
        return _html.HtmlLinkInfo_GetHtmlCell(*args, **kwargs)

    def SetEvent(*args, **kwargs):
        """SetEvent(MouseEvent e)"""
        return _html.HtmlLinkInfo_SetEvent(*args, **kwargs)

    def SetHtmlCell(*args, **kwargs):
        """SetHtmlCell(HtmlCell e)"""
        return _html.HtmlLinkInfo_SetHtmlCell(*args, **kwargs)


class HtmlLinkInfoPtr(HtmlLinkInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlLinkInfo
_html.HtmlLinkInfo_swigregister(HtmlLinkInfoPtr)

class HtmlTag(core.Object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlTag instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _html.HtmlTag_GetName(*args, **kwargs)

    def HasParam(*args, **kwargs):
        """HasParam(wxString par) -> bool"""
        return _html.HtmlTag_HasParam(*args, **kwargs)

    def GetParam(*args, **kwargs):
        """GetParam(wxString par, int with_commas=False) -> wxString"""
        return _html.HtmlTag_GetParam(*args, **kwargs)

    def GetAllParams(*args, **kwargs):
        """GetAllParams() -> wxString"""
        return _html.HtmlTag_GetAllParams(*args, **kwargs)

    def HasEnding(*args, **kwargs):
        """HasEnding() -> bool"""
        return _html.HtmlTag_HasEnding(*args, **kwargs)

    def GetBeginPos(*args, **kwargs):
        """GetBeginPos() -> int"""
        return _html.HtmlTag_GetBeginPos(*args, **kwargs)

    def GetEndPos1(*args, **kwargs):
        """GetEndPos1() -> int"""
        return _html.HtmlTag_GetEndPos1(*args, **kwargs)

    def GetEndPos2(*args, **kwargs):
        """GetEndPos2() -> int"""
        return _html.HtmlTag_GetEndPos2(*args, **kwargs)


class HtmlTagPtr(HtmlTag):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlTag
_html.HtmlTag_swigregister(HtmlTagPtr)

class HtmlParser(core.Object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlParser instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def SetFS(*args, **kwargs):
        """SetFS(FileSystem fs)"""
        return _html.HtmlParser_SetFS(*args, **kwargs)

    def GetFS(*args, **kwargs):
        """GetFS() -> FileSystem"""
        return _html.HtmlParser_GetFS(*args, **kwargs)

    def Parse(*args, **kwargs):
        """Parse(wxString source) -> Object"""
        return _html.HtmlParser_Parse(*args, **kwargs)

    def InitParser(*args, **kwargs):
        """InitParser(wxString source)"""
        return _html.HtmlParser_InitParser(*args, **kwargs)

    def DoneParser(*args, **kwargs):
        """DoneParser()"""
        return _html.HtmlParser_DoneParser(*args, **kwargs)

    def DoParsing(*args, **kwargs):
        """DoParsing(int begin_pos, int end_pos)"""
        return _html.HtmlParser_DoParsing(*args, **kwargs)

    def StopParsing(*args, **kwargs):
        """StopParsing()"""
        return _html.HtmlParser_StopParsing(*args, **kwargs)

    def AddTagHandler(*args, **kwargs):
        """AddTagHandler(wxHtmlTagHandler handler)"""
        return _html.HtmlParser_AddTagHandler(*args, **kwargs)

    def GetSource(*args, **kwargs):
        """GetSource() -> wxString"""
        return _html.HtmlParser_GetSource(*args, **kwargs)

    def PushTagHandler(*args, **kwargs):
        """PushTagHandler(wxHtmlTagHandler handler, wxString tags)"""
        return _html.HtmlParser_PushTagHandler(*args, **kwargs)

    def PopTagHandler(*args, **kwargs):
        """PopTagHandler()"""
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
        """__init__(HtmlWindow wnd=None) -> HtmlWinParser"""
        newobj = _html.new_HtmlWinParser(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetDC(*args, **kwargs):
        """SetDC(wxDC dc)"""
        return _html.HtmlWinParser_SetDC(*args, **kwargs)

    def GetDC(*args, **kwargs):
        """GetDC() -> wxDC"""
        return _html.HtmlWinParser_GetDC(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """GetCharHeight() -> int"""
        return _html.HtmlWinParser_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """GetCharWidth() -> int"""
        return _html.HtmlWinParser_GetCharWidth(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow() -> HtmlWindow"""
        return _html.HtmlWinParser_GetWindow(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(wxString normal_face, wxString fixed_face, PyObject sizes=None)"""
        return _html.HtmlWinParser_SetFonts(*args, **kwargs)

    def GetContainer(*args, **kwargs):
        """GetContainer() -> HtmlContainerCell"""
        return _html.HtmlWinParser_GetContainer(*args, **kwargs)

    def OpenContainer(*args, **kwargs):
        """OpenContainer() -> HtmlContainerCell"""
        return _html.HtmlWinParser_OpenContainer(*args, **kwargs)

    def SetContainer(*args, **kwargs):
        """SetContainer(HtmlContainerCell c) -> HtmlContainerCell"""
        return _html.HtmlWinParser_SetContainer(*args, **kwargs)

    def CloseContainer(*args, **kwargs):
        """CloseContainer() -> HtmlContainerCell"""
        return _html.HtmlWinParser_CloseContainer(*args, **kwargs)

    def GetFontSize(*args, **kwargs):
        """GetFontSize() -> int"""
        return _html.HtmlWinParser_GetFontSize(*args, **kwargs)

    def SetFontSize(*args, **kwargs):
        """SetFontSize(int s)"""
        return _html.HtmlWinParser_SetFontSize(*args, **kwargs)

    def GetFontBold(*args, **kwargs):
        """GetFontBold() -> int"""
        return _html.HtmlWinParser_GetFontBold(*args, **kwargs)

    def SetFontBold(*args, **kwargs):
        """SetFontBold(int x)"""
        return _html.HtmlWinParser_SetFontBold(*args, **kwargs)

    def GetFontItalic(*args, **kwargs):
        """GetFontItalic() -> int"""
        return _html.HtmlWinParser_GetFontItalic(*args, **kwargs)

    def SetFontItalic(*args, **kwargs):
        """SetFontItalic(int x)"""
        return _html.HtmlWinParser_SetFontItalic(*args, **kwargs)

    def GetFontUnderlined(*args, **kwargs):
        """GetFontUnderlined() -> int"""
        return _html.HtmlWinParser_GetFontUnderlined(*args, **kwargs)

    def SetFontUnderlined(*args, **kwargs):
        """SetFontUnderlined(int x)"""
        return _html.HtmlWinParser_SetFontUnderlined(*args, **kwargs)

    def GetFontFixed(*args, **kwargs):
        """GetFontFixed() -> int"""
        return _html.HtmlWinParser_GetFontFixed(*args, **kwargs)

    def SetFontFixed(*args, **kwargs):
        """SetFontFixed(int x)"""
        return _html.HtmlWinParser_SetFontFixed(*args, **kwargs)

    def GetAlign(*args, **kwargs):
        """GetAlign() -> int"""
        return _html.HtmlWinParser_GetAlign(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(int a)"""
        return _html.HtmlWinParser_SetAlign(*args, **kwargs)

    def GetLinkColor(*args, **kwargs):
        """GetLinkColor() -> wxColour"""
        return _html.HtmlWinParser_GetLinkColor(*args, **kwargs)

    def SetLinkColor(*args, **kwargs):
        """SetLinkColor(wxColour clr)"""
        return _html.HtmlWinParser_SetLinkColor(*args, **kwargs)

    def GetActualColor(*args, **kwargs):
        """GetActualColor() -> wxColour"""
        return _html.HtmlWinParser_GetActualColor(*args, **kwargs)

    def SetActualColor(*args, **kwargs):
        """SetActualColor(wxColour clr)"""
        return _html.HtmlWinParser_SetActualColor(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """SetLink(wxString link)"""
        return _html.HtmlWinParser_SetLink(*args, **kwargs)

    def CreateCurrentFont(*args, **kwargs):
        """CreateCurrentFont() -> wxFont"""
        return _html.HtmlWinParser_CreateCurrentFont(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """GetLink() -> HtmlLinkInfo"""
        return _html.HtmlWinParser_GetLink(*args, **kwargs)


class HtmlWinParserPtr(HtmlWinParser):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlWinParser
_html.HtmlWinParser_swigregister(HtmlWinParserPtr)

class HtmlTagHandler(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlTagHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> HtmlTagHandler"""
        newobj = _html.new_HtmlTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlTagHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _html.HtmlTagHandler__setCallbackInfo(*args, **kwargs)

    def SetParser(*args, **kwargs):
        """SetParser(HtmlParser parser)"""
        return _html.HtmlTagHandler_SetParser(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser() -> HtmlParser"""
        return _html.HtmlTagHandler_GetParser(*args, **kwargs)

    def ParseInner(*args, **kwargs):
        """ParseInner(HtmlTag tag)"""
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
        """__init__() -> HtmlWinTagHandler"""
        newobj = _html.new_HtmlWinTagHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWinTagHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _html.HtmlWinTagHandler__setCallbackInfo(*args, **kwargs)

    def SetParser(*args, **kwargs):
        """SetParser(HtmlParser parser)"""
        return _html.HtmlWinTagHandler_SetParser(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser() -> HtmlWinParser"""
        return _html.HtmlWinTagHandler_GetParser(*args, **kwargs)

    def ParseInner(*args, **kwargs):
        """ParseInner(HtmlTag tag)"""
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
        """__init__() -> HtmlSelection"""
        newobj = _html.new_HtmlSelection(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlSelection):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Set(*args, **kwargs):
        """Set(Point fromPos, HtmlCell fromCell, Point toPos, HtmlCell toCell)"""
        return _html.HtmlSelection_Set(*args, **kwargs)

    def SetCells(*args, **kwargs):
        """SetCells(HtmlCell fromCell, HtmlCell toCell)"""
        return _html.HtmlSelection_SetCells(*args, **kwargs)

    def GetFromCell(*args, **kwargs):
        """GetFromCell() -> HtmlCell"""
        return _html.HtmlSelection_GetFromCell(*args, **kwargs)

    def GetToCell(*args, **kwargs):
        """GetToCell() -> HtmlCell"""
        return _html.HtmlSelection_GetToCell(*args, **kwargs)

    def GetFromPos(*args, **kwargs):
        """GetFromPos() -> Point"""
        return _html.HtmlSelection_GetFromPos(*args, **kwargs)

    def GetToPos(*args, **kwargs):
        """GetToPos() -> Point"""
        return _html.HtmlSelection_GetToPos(*args, **kwargs)

    def GetFromPrivPos(*args, **kwargs):
        """GetFromPrivPos() -> Point"""
        return _html.HtmlSelection_GetFromPrivPos(*args, **kwargs)

    def GetToPrivPos(*args, **kwargs):
        """GetToPrivPos() -> Point"""
        return _html.HtmlSelection_GetToPrivPos(*args, **kwargs)

    def SetFromPrivPos(*args, **kwargs):
        """SetFromPrivPos(Point pos)"""
        return _html.HtmlSelection_SetFromPrivPos(*args, **kwargs)

    def SetToPrivPos(*args, **kwargs):
        """SetToPrivPos(Point pos)"""
        return _html.HtmlSelection_SetToPrivPos(*args, **kwargs)

    def ClearPrivPos(*args, **kwargs):
        """ClearPrivPos()"""
        return _html.HtmlSelection_ClearPrivPos(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty() -> bool"""
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
        """__init__() -> HtmlRenderingState"""
        newobj = _html.new_HtmlRenderingState(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingState):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelectionState(*args, **kwargs):
        """SetSelectionState(wxHtmlSelectionState s)"""
        return _html.HtmlRenderingState_SetSelectionState(*args, **kwargs)

    def GetSelectionState(*args, **kwargs):
        """GetSelectionState() -> wxHtmlSelectionState"""
        return _html.HtmlRenderingState_GetSelectionState(*args, **kwargs)

    def SetFgColour(*args, **kwargs):
        """SetFgColour(wxColour c)"""
        return _html.HtmlRenderingState_SetFgColour(*args, **kwargs)

    def GetFgColour(*args, **kwargs):
        """GetFgColour() -> wxColour"""
        return _html.HtmlRenderingState_GetFgColour(*args, **kwargs)

    def SetBgColour(*args, **kwargs):
        """SetBgColour(wxColour c)"""
        return _html.HtmlRenderingState_SetBgColour(*args, **kwargs)

    def GetBgColour(*args, **kwargs):
        """GetBgColour() -> wxColour"""
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
        """GetSelectedTextColour(wxColour clr) -> wxColour"""
        return _html.HtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)

    def GetSelectedTextBgColour(*args, **kwargs):
        """GetSelectedTextBgColour(wxColour clr) -> wxColour"""
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
        """GetSelectedTextColour(wxColour clr) -> wxColour"""
        return _html.DefaultHtmlRenderingStyle_GetSelectedTextColour(*args, **kwargs)

    def GetSelectedTextBgColour(*args, **kwargs):
        """GetSelectedTextBgColour(wxColour clr) -> wxColour"""
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
        """__init__() -> HtmlRenderingInfo"""
        newobj = _html.new_HtmlRenderingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlRenderingInfo):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetSelection(*args, **kwargs):
        """SetSelection(HtmlSelection s)"""
        return _html.HtmlRenderingInfo_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> HtmlSelection"""
        return _html.HtmlRenderingInfo_GetSelection(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(HtmlRenderingStyle style)"""
        return _html.HtmlRenderingInfo_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> HtmlRenderingStyle"""
        return _html.HtmlRenderingInfo_GetStyle(*args, **kwargs)

    def GetState(*args, **kwargs):
        """GetState() -> HtmlRenderingState"""
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
class HtmlCell(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> HtmlCell"""
        newobj = _html.new_HtmlCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosX(*args, **kwargs):
        """GetPosX() -> int"""
        return _html.HtmlCell_GetPosX(*args, **kwargs)

    def GetPosY(*args, **kwargs):
        """GetPosY() -> int"""
        return _html.HtmlCell_GetPosY(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int"""
        return _html.HtmlCell_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight() -> int"""
        return _html.HtmlCell_GetHeight(*args, **kwargs)

    def GetDescent(*args, **kwargs):
        """GetDescent() -> int"""
        return _html.HtmlCell_GetDescent(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """GetLink(int x=0, int y=0) -> HtmlLinkInfo"""
        return _html.HtmlCell_GetLink(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext() -> HtmlCell"""
        return _html.HtmlCell_GetNext(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent() -> HtmlContainerCell"""
        return _html.HtmlCell_GetParent(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild() -> HtmlCell"""
        return _html.HtmlCell_GetFirstChild(*args, **kwargs)

    def GetCursor(*args, **kwargs):
        """GetCursor() -> wxCursor"""
        return _html.HtmlCell_GetCursor(*args, **kwargs)

    def IsFormattingCell(*args, **kwargs):
        """IsFormattingCell() -> bool"""
        return _html.HtmlCell_IsFormattingCell(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """SetLink(HtmlLinkInfo link)"""
        return _html.HtmlCell_SetLink(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(HtmlCell cell)"""
        return _html.HtmlCell_SetNext(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(HtmlContainerCell p)"""
        return _html.HtmlCell_SetParent(*args, **kwargs)

    def SetPos(*args, **kwargs):
        """SetPos(int x, int y)"""
        return _html.HtmlCell_SetPos(*args, **kwargs)

    def Layout(*args, **kwargs):
        """Layout(int w)"""
        return _html.HtmlCell_Layout(*args, **kwargs)

    def Draw(*args, **kwargs):
        """Draw(wxDC dc, int x, int y, int view_y1, int view_y2, HtmlRenderingInfo info)"""
        return _html.HtmlCell_Draw(*args, **kwargs)

    def DrawInvisible(*args, **kwargs):
        """DrawInvisible(wxDC dc, int x, int y, HtmlRenderingInfo info)"""
        return _html.HtmlCell_DrawInvisible(*args, **kwargs)

    def Find(*args, **kwargs):
        """Find(int condition, void param) -> HtmlCell"""
        return _html.HtmlCell_Find(*args, **kwargs)

    def AdjustPagebreak(*args, **kwargs):
        """AdjustPagebreak(int INOUT) -> bool"""
        return _html.HtmlCell_AdjustPagebreak(*args, **kwargs)

    def SetCanLiveOnPagebreak(*args, **kwargs):
        """SetCanLiveOnPagebreak(bool can)"""
        return _html.HtmlCell_SetCanLiveOnPagebreak(*args, **kwargs)

    def IsLinebreakAllowed(*args, **kwargs):
        """IsLinebreakAllowed() -> bool"""
        return _html.HtmlCell_IsLinebreakAllowed(*args, **kwargs)

    def IsTerminalCell(*args, **kwargs):
        """IsTerminalCell() -> bool"""
        return _html.HtmlCell_IsTerminalCell(*args, **kwargs)

    def FindCellByPos(*args, **kwargs):
        """FindCellByPos(int x, int y, unsigned int flags=HTML_FIND_EXACT) -> HtmlCell"""
        return _html.HtmlCell_FindCellByPos(*args, **kwargs)

    def GetAbsPos(*args, **kwargs):
        """GetAbsPos() -> Point"""
        return _html.HtmlCell_GetAbsPos(*args, **kwargs)

    def GetFirstTerminal(*args, **kwargs):
        """GetFirstTerminal() -> HtmlCell"""
        return _html.HtmlCell_GetFirstTerminal(*args, **kwargs)

    def GetLastTerminal(*args, **kwargs):
        """GetLastTerminal() -> HtmlCell"""
        return _html.HtmlCell_GetLastTerminal(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth() -> unsigned int"""
        return _html.HtmlCell_GetDepth(*args, **kwargs)

    def IsBefore(*args, **kwargs):
        """IsBefore(HtmlCell cell) -> bool"""
        return _html.HtmlCell_IsBefore(*args, **kwargs)

    def ConvertToText(*args, **kwargs):
        """ConvertToText(HtmlSelection sel) -> wxString"""
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
        """__init__(wxString word, wxDC dc) -> HtmlWordCell"""
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
        """__init__(HtmlContainerCell parent) -> HtmlContainerCell"""
        newobj = _html.new_HtmlContainerCell(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def InsertCell(*args, **kwargs):
        """InsertCell(HtmlCell cell)"""
        return _html.HtmlContainerCell_InsertCell(*args, **kwargs)

    def SetAlignHor(*args, **kwargs):
        """SetAlignHor(int al)"""
        return _html.HtmlContainerCell_SetAlignHor(*args, **kwargs)

    def GetAlignHor(*args, **kwargs):
        """GetAlignHor() -> int"""
        return _html.HtmlContainerCell_GetAlignHor(*args, **kwargs)

    def SetAlignVer(*args, **kwargs):
        """SetAlignVer(int al)"""
        return _html.HtmlContainerCell_SetAlignVer(*args, **kwargs)

    def GetAlignVer(*args, **kwargs):
        """GetAlignVer() -> int"""
        return _html.HtmlContainerCell_GetAlignVer(*args, **kwargs)

    def SetIndent(*args, **kwargs):
        """SetIndent(int i, int what, int units=HTML_UNITS_PIXELS)"""
        return _html.HtmlContainerCell_SetIndent(*args, **kwargs)

    def GetIndent(*args, **kwargs):
        """GetIndent(int ind) -> int"""
        return _html.HtmlContainerCell_GetIndent(*args, **kwargs)

    def GetIndentUnits(*args, **kwargs):
        """GetIndentUnits(int ind) -> int"""
        return _html.HtmlContainerCell_GetIndentUnits(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(HtmlTag tag)"""
        return _html.HtmlContainerCell_SetAlign(*args, **kwargs)

    def SetWidthFloat(*args, **kwargs):
        """SetWidthFloat(int w, int units)"""
        return _html.HtmlContainerCell_SetWidthFloat(*args, **kwargs)

    def SetWidthFloatFromTag(*args, **kwargs):
        """SetWidthFloatFromTag(HtmlTag tag)"""
        return _html.HtmlContainerCell_SetWidthFloatFromTag(*args, **kwargs)

    def SetMinHeight(*args, **kwargs):
        """SetMinHeight(int h, int align=HTML_ALIGN_TOP)"""
        return _html.HtmlContainerCell_SetMinHeight(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour clr)"""
        return _html.HtmlContainerCell_SetBackgroundColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour() -> wxColour"""
        return _html.HtmlContainerCell_GetBackgroundColour(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """SetBorder(wxColour clr1, wxColour clr2)"""
        return _html.HtmlContainerCell_SetBorder(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild() -> HtmlCell"""
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
        """__init__(wxColour clr, int flags=HTML_CLR_FOREGROUND) -> HtmlColourCell"""
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
        """__init__(wxFont font) -> HtmlFontCell"""
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
        """__init__(Window wnd, int w=0) -> HtmlWidgetCell"""
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

class HtmlFilter(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlFilter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> HtmlFilter"""
        newobj = _html.new_HtmlFilter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlFilter)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _html.HtmlFilter__setCallbackInfo(*args, **kwargs)


class HtmlFilterPtr(HtmlFilter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlFilter
_html.HtmlFilter_swigregister(HtmlFilterPtr)

#---------------------------------------------------------------------------

class HtmlWindow(windows.ScrolledWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, int style=HW_DEFAULT_STYLE, 
            wxString name=wxPyHtmlWindowNameStr) -> HtmlWindow
        """
        newobj = _html.new_HtmlWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, HtmlWindow); self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, int style=HW_SCROLLBAR_AUTO, 
            wxString name=wxPyHtmlWindowNameStr) -> bool
        """
        return _html.HtmlWindow_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _html.HtmlWindow__setCallbackInfo(*args, **kwargs)

    def SetPage(*args, **kwargs):
        """SetPage(wxString source) -> bool"""
        return _html.HtmlWindow_SetPage(*args, **kwargs)

    def LoadPage(*args, **kwargs):
        """LoadPage(wxString location) -> bool"""
        return _html.HtmlWindow_LoadPage(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(wxString filename) -> bool"""
        return _html.HtmlWindow_LoadFile(*args, **kwargs)

    def AppendToPage(*args, **kwargs):
        """AppendToPage(wxString source) -> bool"""
        return _html.HtmlWindow_AppendToPage(*args, **kwargs)

    def GetOpenedPage(*args, **kwargs):
        """GetOpenedPage() -> wxString"""
        return _html.HtmlWindow_GetOpenedPage(*args, **kwargs)

    def GetOpenedAnchor(*args, **kwargs):
        """GetOpenedAnchor() -> wxString"""
        return _html.HtmlWindow_GetOpenedAnchor(*args, **kwargs)

    def GetOpenedPageTitle(*args, **kwargs):
        """GetOpenedPageTitle() -> wxString"""
        return _html.HtmlWindow_GetOpenedPageTitle(*args, **kwargs)

    def SetRelatedFrame(*args, **kwargs):
        """SetRelatedFrame(Frame frame, wxString format)"""
        return _html.HtmlWindow_SetRelatedFrame(*args, **kwargs)

    def GetRelatedFrame(*args, **kwargs):
        """GetRelatedFrame() -> Frame"""
        return _html.HtmlWindow_GetRelatedFrame(*args, **kwargs)

    def SetRelatedStatusBar(*args, **kwargs):
        """SetRelatedStatusBar(int bar)"""
        return _html.HtmlWindow_SetRelatedStatusBar(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(wxString normal_face, wxString fixed_face, PyObject sizes=None)"""
        return _html.HtmlWindow_SetFonts(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(wxString title)"""
        return _html.HtmlWindow_SetTitle(*args, **kwargs)

    def SetBorders(*args, **kwargs):
        """SetBorders(int b)"""
        return _html.HtmlWindow_SetBorders(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
        return _html.HtmlWindow_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
        return _html.HtmlWindow_WriteCustomization(*args, **kwargs)

    def HistoryBack(*args, **kwargs):
        """HistoryBack() -> bool"""
        return _html.HtmlWindow_HistoryBack(*args, **kwargs)

    def HistoryForward(*args, **kwargs):
        """HistoryForward() -> bool"""
        return _html.HtmlWindow_HistoryForward(*args, **kwargs)

    def HistoryCanBack(*args, **kwargs):
        """HistoryCanBack() -> bool"""
        return _html.HtmlWindow_HistoryCanBack(*args, **kwargs)

    def HistoryCanForward(*args, **kwargs):
        """HistoryCanForward() -> bool"""
        return _html.HtmlWindow_HistoryCanForward(*args, **kwargs)

    def HistoryClear(*args, **kwargs):
        """HistoryClear()"""
        return _html.HtmlWindow_HistoryClear(*args, **kwargs)

    def GetInternalRepresentation(*args, **kwargs):
        """GetInternalRepresentation() -> HtmlContainerCell"""
        return _html.HtmlWindow_GetInternalRepresentation(*args, **kwargs)

    def GetParser(*args, **kwargs):
        """GetParser() -> HtmlWinParser"""
        return _html.HtmlWindow_GetParser(*args, **kwargs)

    def ScrollToAnchor(*args, **kwargs):
        """ScrollToAnchor(wxString anchor) -> bool"""
        return _html.HtmlWindow_ScrollToAnchor(*args, **kwargs)

    def HasAnchor(*args, **kwargs):
        """HasAnchor(wxString anchor) -> bool"""
        return _html.HtmlWindow_HasAnchor(*args, **kwargs)

    def AddFilter(*args, **kwargs):
        """HtmlWindow.AddFilter(HtmlFilter filter)"""
        return _html.HtmlWindow_AddFilter(*args, **kwargs)

    AddFilter = staticmethod(AddFilter)
    def base_OnLinkClicked(*args, **kwargs):
        """base_OnLinkClicked(HtmlLinkInfo link)"""
        return _html.HtmlWindow_base_OnLinkClicked(*args, **kwargs)

    def base_OnSetTitle(*args, **kwargs):
        """base_OnSetTitle(wxString title)"""
        return _html.HtmlWindow_base_OnSetTitle(*args, **kwargs)

    def base_OnCellMouseHover(*args, **kwargs):
        """base_OnCellMouseHover(HtmlCell cell, int x, int y)"""
        return _html.HtmlWindow_base_OnCellMouseHover(*args, **kwargs)

    def base_OnCellClicked(*args, **kwargs):
        """base_OnCellClicked(HtmlCell cell, int x, int y, MouseEvent event)"""
        return _html.HtmlWindow_base_OnCellClicked(*args, **kwargs)


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

#---------------------------------------------------------------------------

class HtmlDCRenderer(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlDCRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> HtmlDCRenderer"""
        newobj = _html.new_HtmlDCRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlDCRenderer):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetDC(*args, **kwargs):
        """SetDC(wxDC dc, int maxwidth)"""
        return _html.HtmlDCRenderer_SetDC(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(int width, int height)"""
        return _html.HtmlDCRenderer_SetSize(*args, **kwargs)

    def SetHtmlText(*args, **kwargs):
        """SetHtmlText(wxString html, wxString basepath=wxPyEmptyString, bool isdir=True)"""
        return _html.HtmlDCRenderer_SetHtmlText(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(wxString normal_face, wxString fixed_face, PyObject sizes=None)"""
        return _html.HtmlDCRenderer_SetFonts(*args, **kwargs)

    def Render(*args, **kwargs):
        """
        Render(int x, int y, int from=0, int dont_render=False, int to=INT_MAX, 
            int choices=None, int LCOUNT=0) -> int
        """
        return _html.HtmlDCRenderer_Render(*args, **kwargs)

    def GetTotalHeight(*args, **kwargs):
        """GetTotalHeight() -> int"""
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
class HtmlPrintout(windows.Printout):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlPrintout instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString title=wxPyHtmlPrintoutTitleStr) -> HtmlPrintout"""
        newobj = _html.new_HtmlPrintout(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetHtmlText(*args, **kwargs):
        """SetHtmlText(wxString html, wxString basepath=wxPyEmptyString, bool isdir=True)"""
        return _html.HtmlPrintout_SetHtmlText(*args, **kwargs)

    def SetHtmlFile(*args, **kwargs):
        """SetHtmlFile(wxString htmlfile)"""
        return _html.HtmlPrintout_SetHtmlFile(*args, **kwargs)

    def SetHeader(*args, **kwargs):
        """SetHeader(wxString header, int pg=PAGE_ALL)"""
        return _html.HtmlPrintout_SetHeader(*args, **kwargs)

    def SetFooter(*args, **kwargs):
        """SetFooter(wxString footer, int pg=PAGE_ALL)"""
        return _html.HtmlPrintout_SetFooter(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(wxString normal_face, wxString fixed_face, PyObject sizes=None)"""
        return _html.HtmlPrintout_SetFonts(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """
        SetMargins(float top=25.2, float bottom=25.2, float left=25.2, 
            float right=25.2, float spaces=5)
        """
        return _html.HtmlPrintout_SetMargins(*args, **kwargs)

    def AddFilter(*args, **kwargs):
        """HtmlPrintout.AddFilter(wxHtmlFilter filter)"""
        return _html.HtmlPrintout_AddFilter(*args, **kwargs)

    AddFilter = staticmethod(AddFilter)
    def CleanUpStatics(*args, **kwargs):
        """HtmlPrintout.CleanUpStatics()"""
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

class HtmlEasyPrinting(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlEasyPrinting instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString name=wxPyHtmlPrintingTitleStr, Window parentWindow=None) -> HtmlEasyPrinting"""
        newobj = _html.new_HtmlEasyPrinting(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlEasyPrinting):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def PreviewFile(*args, **kwargs):
        """PreviewFile(wxString htmlfile)"""
        return _html.HtmlEasyPrinting_PreviewFile(*args, **kwargs)

    def PreviewText(*args, **kwargs):
        """PreviewText(wxString htmltext, wxString basepath=wxPyEmptyString)"""
        return _html.HtmlEasyPrinting_PreviewText(*args, **kwargs)

    def PrintFile(*args, **kwargs):
        """PrintFile(wxString htmlfile)"""
        return _html.HtmlEasyPrinting_PrintFile(*args, **kwargs)

    def PrintText(*args, **kwargs):
        """PrintText(wxString htmltext, wxString basepath=wxPyEmptyString)"""
        return _html.HtmlEasyPrinting_PrintText(*args, **kwargs)

    def PrinterSetup(*args, **kwargs):
        """PrinterSetup()"""
        return _html.HtmlEasyPrinting_PrinterSetup(*args, **kwargs)

    def PageSetup(*args, **kwargs):
        """PageSetup()"""
        return _html.HtmlEasyPrinting_PageSetup(*args, **kwargs)

    def SetHeader(*args, **kwargs):
        """SetHeader(wxString header, int pg=PAGE_ALL)"""
        return _html.HtmlEasyPrinting_SetHeader(*args, **kwargs)

    def SetFooter(*args, **kwargs):
        """SetFooter(wxString footer, int pg=PAGE_ALL)"""
        return _html.HtmlEasyPrinting_SetFooter(*args, **kwargs)

    def SetFonts(*args, **kwargs):
        """SetFonts(wxString normal_face, wxString fixed_face, PyObject sizes=None)"""
        return _html.HtmlEasyPrinting_SetFonts(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData() -> PrintData"""
        return _html.HtmlEasyPrinting_GetPrintData(*args, **kwargs)

    def GetPageSetupData(*args, **kwargs):
        """GetPageSetupData() -> PageSetupDialogData"""
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
        """
        __init__(wxString bookfile, wxString basepath, wxString title, 
            wxString start) -> HtmlBookRecord
        """
        newobj = _html.new_HtmlBookRecord(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetBookFile(*args, **kwargs):
        """GetBookFile() -> wxString"""
        return _html.HtmlBookRecord_GetBookFile(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle() -> wxString"""
        return _html.HtmlBookRecord_GetTitle(*args, **kwargs)

    def GetStart(*args, **kwargs):
        """GetStart() -> wxString"""
        return _html.HtmlBookRecord_GetStart(*args, **kwargs)

    def GetBasePath(*args, **kwargs):
        """GetBasePath() -> wxString"""
        return _html.HtmlBookRecord_GetBasePath(*args, **kwargs)

    def SetContentsRange(*args, **kwargs):
        """SetContentsRange(int start, int end)"""
        return _html.HtmlBookRecord_SetContentsRange(*args, **kwargs)

    def GetContentsStart(*args, **kwargs):
        """GetContentsStart() -> int"""
        return _html.HtmlBookRecord_GetContentsStart(*args, **kwargs)

    def GetContentsEnd(*args, **kwargs):
        """GetContentsEnd() -> int"""
        return _html.HtmlBookRecord_GetContentsEnd(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(wxString title)"""
        return _html.HtmlBookRecord_SetTitle(*args, **kwargs)

    def SetBasePath(*args, **kwargs):
        """SetBasePath(wxString path)"""
        return _html.HtmlBookRecord_SetBasePath(*args, **kwargs)

    def SetStart(*args, **kwargs):
        """SetStart(wxString start)"""
        return _html.HtmlBookRecord_SetStart(*args, **kwargs)

    def GetFullPath(*args, **kwargs):
        """GetFullPath(wxString page) -> wxString"""
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
        """GetLevel() -> int"""
        return _html.HtmlContentsItem_GetLevel(*args, **kwargs)

    def GetID(*args, **kwargs):
        """GetID() -> int"""
        return _html.HtmlContentsItem_GetID(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _html.HtmlContentsItem_GetName(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage() -> wxString"""
        return _html.HtmlContentsItem_GetPage(*args, **kwargs)

    def GetBook(*args, **kwargs):
        """GetBook() -> HtmlBookRecord"""
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
        """Search() -> bool"""
        return _html.HtmlSearchStatus_Search(*args, **kwargs)

    def IsActive(*args, **kwargs):
        """IsActive() -> bool"""
        return _html.HtmlSearchStatus_IsActive(*args, **kwargs)

    def GetCurIndex(*args, **kwargs):
        """GetCurIndex() -> int"""
        return _html.HtmlSearchStatus_GetCurIndex(*args, **kwargs)

    def GetMaxIndex(*args, **kwargs):
        """GetMaxIndex() -> int"""
        return _html.HtmlSearchStatus_GetMaxIndex(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _html.HtmlSearchStatus_GetName(*args, **kwargs)

    def GetContentsItem(*args, **kwargs):
        """GetContentsItem() -> HtmlContentsItem"""
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
        """__init__() -> HtmlHelpData"""
        newobj = _html.new_HtmlHelpData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_html.delete_HtmlHelpData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetTempDir(*args, **kwargs):
        """SetTempDir(wxString path)"""
        return _html.HtmlHelpData_SetTempDir(*args, **kwargs)

    def AddBook(*args, **kwargs):
        """AddBook(wxString book) -> bool"""
        return _html.HtmlHelpData_AddBook(*args, **kwargs)

    def FindPageByName(*args, **kwargs):
        """FindPageByName(wxString page) -> wxString"""
        return _html.HtmlHelpData_FindPageByName(*args, **kwargs)

    def FindPageById(*args, **kwargs):
        """FindPageById(int id) -> wxString"""
        return _html.HtmlHelpData_FindPageById(*args, **kwargs)

    def GetBookRecArray(*args, **kwargs):
        """GetBookRecArray() -> wxHtmlBookRecArray"""
        return _html.HtmlHelpData_GetBookRecArray(*args, **kwargs)

    def GetContents(*args, **kwargs):
        """GetContents() -> HtmlContentsItem"""
        return _html.HtmlHelpData_GetContents(*args, **kwargs)

    def GetContentsCnt(*args, **kwargs):
        """GetContentsCnt() -> int"""
        return _html.HtmlHelpData_GetContentsCnt(*args, **kwargs)

    def GetIndex(*args, **kwargs):
        """GetIndex() -> HtmlContentsItem"""
        return _html.HtmlHelpData_GetIndex(*args, **kwargs)

    def GetIndexCnt(*args, **kwargs):
        """GetIndexCnt() -> int"""
        return _html.HtmlHelpData_GetIndexCnt(*args, **kwargs)


class HtmlHelpDataPtr(HtmlHelpData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpData
_html.HtmlHelpData_swigregister(HtmlHelpDataPtr)

class HtmlHelpFrame(windows.Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int ??, wxString title=wxPyEmptyString, 
            int style=HF_DEFAULTSTYLE, HtmlHelpData data=None) -> HtmlHelpFrame
        """
        newobj = _html.new_HtmlHelpFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetData(*args, **kwargs):
        """GetData() -> HtmlHelpData"""
        return _html.HtmlHelpFrame_GetData(*args, **kwargs)

    def SetTitleFormat(*args, **kwargs):
        """SetTitleFormat(wxString format)"""
        return _html.HtmlHelpFrame_SetTitleFormat(*args, **kwargs)

    def Display(*args, **kwargs):
        """Display(wxString x)"""
        return _html.HtmlHelpFrame_Display(*args, **kwargs)

    def DisplayID(*args, **kwargs):
        """DisplayID(int id)"""
        return _html.HtmlHelpFrame_DisplayID(*args, **kwargs)

    def DisplayContents(*args, **kwargs):
        """DisplayContents()"""
        return _html.HtmlHelpFrame_DisplayContents(*args, **kwargs)

    def DisplayIndex(*args, **kwargs):
        """DisplayIndex()"""
        return _html.HtmlHelpFrame_DisplayIndex(*args, **kwargs)

    def KeywordSearch(*args, **kwargs):
        """KeywordSearch(wxString keyword) -> bool"""
        return _html.HtmlHelpFrame_KeywordSearch(*args, **kwargs)

    def UseConfig(*args, **kwargs):
        """UseConfig(wxConfigBase config, wxString rootpath=wxPyEmptyString)"""
        return _html.HtmlHelpFrame_UseConfig(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
        return _html.HtmlHelpFrame_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
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
class HtmlHelpController(core.EvtHandler):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHtmlHelpController instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int style=HF_DEFAULTSTYLE) -> HtmlHelpController"""
        newobj = _html.new_HtmlHelpController(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def __del__(self, destroy=_html.delete_HtmlHelpController):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetTitleFormat(*args, **kwargs):
        """SetTitleFormat(wxString format)"""
        return _html.HtmlHelpController_SetTitleFormat(*args, **kwargs)

    def SetTempDir(*args, **kwargs):
        """SetTempDir(wxString path)"""
        return _html.HtmlHelpController_SetTempDir(*args, **kwargs)

    def AddBook(*args, **kwargs):
        """AddBook(wxString book, int show_wait_msg=False) -> bool"""
        return _html.HtmlHelpController_AddBook(*args, **kwargs)

    def Display(*args, **kwargs):
        """Display(wxString x)"""
        return _html.HtmlHelpController_Display(*args, **kwargs)

    def DisplayID(*args, **kwargs):
        """DisplayID(int id)"""
        return _html.HtmlHelpController_DisplayID(*args, **kwargs)

    def DisplayContents(*args, **kwargs):
        """DisplayContents()"""
        return _html.HtmlHelpController_DisplayContents(*args, **kwargs)

    def DisplayIndex(*args, **kwargs):
        """DisplayIndex()"""
        return _html.HtmlHelpController_DisplayIndex(*args, **kwargs)

    def KeywordSearch(*args, **kwargs):
        """KeywordSearch(wxString keyword) -> bool"""
        return _html.HtmlHelpController_KeywordSearch(*args, **kwargs)

    def UseConfig(*args, **kwargs):
        """UseConfig(wxConfigBase config, wxString rootpath=wxPyEmptyString)"""
        return _html.HtmlHelpController_UseConfig(*args, **kwargs)

    def ReadCustomization(*args, **kwargs):
        """ReadCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
        return _html.HtmlHelpController_ReadCustomization(*args, **kwargs)

    def WriteCustomization(*args, **kwargs):
        """WriteCustomization(wxConfigBase cfg, wxString path=wxPyEmptyString)"""
        return _html.HtmlHelpController_WriteCustomization(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame() -> HtmlHelpFrame"""
        return _html.HtmlHelpController_GetFrame(*args, **kwargs)


class HtmlHelpControllerPtr(HtmlHelpController):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlHelpController
_html.HtmlHelpController_swigregister(HtmlHelpControllerPtr)


