# This file was created automatically by SWIG.
import stc_c

from misc import *

from misc2 import *

from windows import *

from gdi import *

from clip_dnd import *

from events import *

from streams import *

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

from utils import *

def EVT_STC_CHANGE(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHANGE, fn)

def EVT_STC_STYLENEEDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_STYLENEEDED, fn)

def EVT_STC_CHARADDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHARADDED, fn)

def EVT_STC_UPDATEUI(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_UPDATEUI, fn)

def EVT_STC_SAVEPOINTREACHED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTREACHED, fn)

def EVT_STC_SAVEPOINTLEFT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTLEFT, fn)

def EVT_STC_ROMODIFYATTEMPT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_ROMODIFYATTEMPT, fn)

def EVT_STC_DOUBLECLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_DOUBLECLICK, fn)

def EVT_STC_MODIFIED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MODIFIED, fn)

def EVT_STC_KEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_KEY, fn)

def EVT_STC_MACRORECORD(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MACRORECORD, fn)

def EVT_STC_MARGINCLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MARGINCLICK, fn)

def EVT_STC_NEEDSHOWN(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_NEEDSHOWN, fn)

def EVT_STC_POSCHANGED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_POSCHANGED, fn)



class wxStyledTextCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AddText,(self,) + _args, _kwargs)
        return val
    def AddStyledText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AddStyledText,(self,) + _args, _kwargs)
        return val
    def InsertText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_InsertText,(self,) + _args, _kwargs)
        return val
    def ClearAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ClearAll,(self,) + _args, _kwargs)
        return val
    def ClearDocumentStyle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ClearDocumentStyle,(self,) + _args, _kwargs)
        return val
    def GetLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLength,(self,) + _args, _kwargs)
        return val
    def GetCharAt(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCharAt,(self,) + _args, _kwargs)
        return val
    def GetCurrentPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurrentPos,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetAnchor,(self,) + _args, _kwargs)
        return val
    def GetStyleAt(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyleAt,(self,) + _args, _kwargs)
        return val
    def Redo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Redo,(self,) + _args, _kwargs)
        return val
    def SetUndoCollection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetUndoCollection,(self,) + _args, _kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SelectAll,(self,) + _args, _kwargs)
        return val
    def SetSavePoint(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSavePoint,(self,) + _args, _kwargs)
        return val
    def GetStyledText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyledText,(self,) + _args, _kwargs)
        return val
    def CanRedo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanRedo,(self,) + _args, _kwargs)
        return val
    def MarkerLineFromHandle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerLineFromHandle,(self,) + _args, _kwargs)
        return val
    def MarkerDeleteHandle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDeleteHandle,(self,) + _args, _kwargs)
        return val
    def GetUndoCollection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetUndoCollection,(self,) + _args, _kwargs)
        return val
    def GetViewWhiteSpace(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetViewWhiteSpace,(self,) + _args, _kwargs)
        return val
    def SetViewWhiteSpace(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetViewWhiteSpace,(self,) + _args, _kwargs)
        return val
    def PositionFromPoint(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PositionFromPoint,(self,) + _args, _kwargs)
        return val
    def GotoLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GotoLine,(self,) + _args, _kwargs)
        return val
    def GotoPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GotoPos,(self,) + _args, _kwargs)
        return val
    def SetAnchor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetAnchor,(self,) + _args, _kwargs)
        return val
    def GetCurLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurLine,(self,) + _args, _kwargs)
        return val
    def GetEndStyled(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEndStyled,(self,) + _args, _kwargs)
        return val
    def ConvertEOLs(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ConvertEOLs,(self,) + _args, _kwargs)
        return val
    def GetEOLMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEOLMode,(self,) + _args, _kwargs)
        return val
    def SetEOLMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetEOLMode,(self,) + _args, _kwargs)
        return val
    def StartStyling(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StartStyling,(self,) + _args, _kwargs)
        return val
    def SetStyling(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyling,(self,) + _args, _kwargs)
        return val
    def GetBufferedDraw(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetBufferedDraw,(self,) + _args, _kwargs)
        return val
    def SetBufferedDraw(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetBufferedDraw,(self,) + _args, _kwargs)
        return val
    def SetTabWidth(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetTabWidth,(self,) + _args, _kwargs)
        return val
    def GetTabWidth(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetTabWidth,(self,) + _args, _kwargs)
        return val
    def SetCodePage(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCodePage,(self,) + _args, _kwargs)
        return val
    def MarkerDefine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDefine,(self,) + _args, _kwargs)
        return val
    def MarkerSetForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerSetForeground,(self,) + _args, _kwargs)
        return val
    def MarkerSetBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerSetBackground,(self,) + _args, _kwargs)
        return val
    def MarkerAdd(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerAdd,(self,) + _args, _kwargs)
        return val
    def MarkerDelete(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDelete,(self,) + _args, _kwargs)
        return val
    def MarkerDeleteAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDeleteAll,(self,) + _args, _kwargs)
        return val
    def MarkerGet(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerGet,(self,) + _args, _kwargs)
        return val
    def MarkerNext(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerNext,(self,) + _args, _kwargs)
        return val
    def MarkerPrevious(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerPrevious,(self,) + _args, _kwargs)
        return val
    def SetMarginType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginType,(self,) + _args, _kwargs)
        return val
    def GetMarginType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginType,(self,) + _args, _kwargs)
        return val
    def SetMarginWidth(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginWidth,(self,) + _args, _kwargs)
        return val
    def GetMarginWidth(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginWidth,(self,) + _args, _kwargs)
        return val
    def SetMarginMask(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginMask,(self,) + _args, _kwargs)
        return val
    def GetMarginMask(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginMask,(self,) + _args, _kwargs)
        return val
    def SetMarginSensitive(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginSensitive,(self,) + _args, _kwargs)
        return val
    def GetMarginSensitive(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginSensitive,(self,) + _args, _kwargs)
        return val
    def StyleClearAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleClearAll,(self,) + _args, _kwargs)
        return val
    def StyleSetForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetForeground,(self,) + _args, _kwargs)
        return val
    def StyleSetBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetBackground,(self,) + _args, _kwargs)
        return val
    def StyleSetBold(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetBold,(self,) + _args, _kwargs)
        return val
    def StyleSetItalic(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetItalic,(self,) + _args, _kwargs)
        return val
    def StyleSetSize(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetSize,(self,) + _args, _kwargs)
        return val
    def StyleSetFaceName(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFaceName,(self,) + _args, _kwargs)
        return val
    def StyleSetEOLFilled(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetEOLFilled,(self,) + _args, _kwargs)
        return val
    def StyleResetDefault(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleResetDefault,(self,) + _args, _kwargs)
        return val
    def StyleSetUnderline(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetUnderline,(self,) + _args, _kwargs)
        return val
    def SetSelForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelForeground,(self,) + _args, _kwargs)
        return val
    def SetSelBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelBackground,(self,) + _args, _kwargs)
        return val
    def SetCaretForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretForeground,(self,) + _args, _kwargs)
        return val
    def CmdKeyAssign(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyAssign,(self,) + _args, _kwargs)
        return val
    def CmdKeyClear(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyClear,(self,) + _args, _kwargs)
        return val
    def CmdKeyClearAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyClearAll,(self,) + _args, _kwargs)
        return val
    def SetStyleBytes(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyleBytes,(self,) + _args, _kwargs)
        return val
    def StyleSetVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetVisible,(self,) + _args, _kwargs)
        return val
    def GetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetWordChars(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetWordChars,(self,) + _args, _kwargs)
        return val
    def BeginUndoAction(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BeginUndoAction,(self,) + _args, _kwargs)
        return val
    def EndUndoAction(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EndUndoAction,(self,) + _args, _kwargs)
        return val
    def IndicatorSetStyle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorSetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorGetStyle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorGetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorSetForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorSetForeground,(self,) + _args, _kwargs)
        return val
    def IndicatorGetForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorGetForeground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetStyleBits(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyleBits,(self,) + _args, _kwargs)
        return val
    def GetStyleBits(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyleBits,(self,) + _args, _kwargs)
        return val
    def SetLineState(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetLineState,(self,) + _args, _kwargs)
        return val
    def GetLineState(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineState,(self,) + _args, _kwargs)
        return val
    def GetMaxLineState(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMaxLineState,(self,) + _args, _kwargs)
        return val
    def AutoCompShow(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompShow,(self,) + _args, _kwargs)
        return val
    def AutoCompCancel(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompCancel,(self,) + _args, _kwargs)
        return val
    def AutoCompActive(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompActive,(self,) + _args, _kwargs)
        return val
    def AutoCompPosStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompPosStart,(self,) + _args, _kwargs)
        return val
    def AutoCompComplete(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompComplete,(self,) + _args, _kwargs)
        return val
    def AutoCompStops(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompStops,(self,) + _args, _kwargs)
        return val
    def AutoCompSetSeparator(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSetSeparator,(self,) + _args, _kwargs)
        return val
    def AutoCompGetSeparator(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompGetSeparator,(self,) + _args, _kwargs)
        return val
    def AutoCompSelect(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSelect,(self,) + _args, _kwargs)
        return val
    def AutoCompSetCancelAtStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSetCancelAtStart,(self,) + _args, _kwargs)
        return val
    def AutoCompGetCancelAtStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompGetCancelAtStart,(self,) + _args, _kwargs)
        return val
    def AutoCompSetFillUps(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSetFillUps,(self,) + _args, _kwargs)
        return val
    def AutoCompSetChooseSingle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSetChooseSingle,(self,) + _args, _kwargs)
        return val
    def AutoCompGetChooseSingle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompGetChooseSingle,(self,) + _args, _kwargs)
        return val
    def AutoCompSetIgnoreCase(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompSetIgnoreCase,(self,) + _args, _kwargs)
        return val
    def AutoCompGetIgnoreCase(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompGetIgnoreCase,(self,) + _args, _kwargs)
        return val
    def SetIndent(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetIndent,(self,) + _args, _kwargs)
        return val
    def GetIndent(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetIndent,(self,) + _args, _kwargs)
        return val
    def SetUseTabs(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetUseTabs,(self,) + _args, _kwargs)
        return val
    def GetUseTabs(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetUseTabs,(self,) + _args, _kwargs)
        return val
    def SetLineIndentation(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetLineIndentation,(self,) + _args, _kwargs)
        return val
    def GetLineIndentation(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineIndentation,(self,) + _args, _kwargs)
        return val
    def GetLineIndentPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineIndentPosition,(self,) + _args, _kwargs)
        return val
    def GetColumn(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetColumn,(self,) + _args, _kwargs)
        return val
    def SetUseHorizontalScrollBar(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetUseHorizontalScrollBar,(self,) + _args, _kwargs)
        return val
    def GetUseHorizontalScrollBar(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetUseHorizontalScrollBar,(self,) + _args, _kwargs)
        return val
    def SetIndentationGuides(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetIndentationGuides,(self,) + _args, _kwargs)
        return val
    def GetIndentationGuides(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetIndentationGuides,(self,) + _args, _kwargs)
        return val
    def SetHighlightGuide(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetHighlightGuide,(self,) + _args, _kwargs)
        return val
    def GetHighlightGuide(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetHighlightGuide,(self,) + _args, _kwargs)
        return val
    def GetLineEndPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineEndPosition,(self,) + _args, _kwargs)
        return val
    def GetCodePage(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCodePage,(self,) + _args, _kwargs)
        return val
    def GetCaretForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCaretForeground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetReadOnly(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetReadOnly,(self,) + _args, _kwargs)
        return val
    def SetCurrentPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCurrentPos,(self,) + _args, _kwargs)
        return val
    def SetSelectionStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelectionStart,(self,) + _args, _kwargs)
        return val
    def GetSelectionStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelectionStart,(self,) + _args, _kwargs)
        return val
    def SetSelectionEnd(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelectionEnd,(self,) + _args, _kwargs)
        return val
    def GetSelectionEnd(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelectionEnd,(self,) + _args, _kwargs)
        return val
    def SetPrintMagnification(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetPrintMagnification,(self,) + _args, _kwargs)
        return val
    def GetPrintMagnification(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetPrintMagnification,(self,) + _args, _kwargs)
        return val
    def SetPrintColourMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetPrintColourMode,(self,) + _args, _kwargs)
        return val
    def GetPrintColourMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetPrintColourMode,(self,) + _args, _kwargs)
        return val
    def FindText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_FindText,(self,) + _args, _kwargs)
        return val
    def FormatRange(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_FormatRange,(self,) + _args, _kwargs)
        return val
    def GetFirstVisibleLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetFirstVisibleLine,(self,) + _args, _kwargs)
        return val
    def GetLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLine,(self,) + _args, _kwargs)
        return val
    def GetLineCount(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineCount,(self,) + _args, _kwargs)
        return val
    def SetMarginLeft(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginLeft,(self,) + _args, _kwargs)
        return val
    def GetMarginLeft(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginLeft,(self,) + _args, _kwargs)
        return val
    def SetMarginRight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMarginRight,(self,) + _args, _kwargs)
        return val
    def GetMarginRight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetMarginRight,(self,) + _args, _kwargs)
        return val
    def GetModify(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetModify,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelection,(self,) + _args, _kwargs)
        return val
    def GetSelectedText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelectedText,(self,) + _args, _kwargs)
        return val
    def GetTextRange(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetTextRange,(self,) + _args, _kwargs)
        return val
    def HideSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_HideSelection,(self,) + _args, _kwargs)
        return val
    def LineFromPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_LineFromPosition,(self,) + _args, _kwargs)
        return val
    def PositionFromLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PositionFromLine,(self,) + _args, _kwargs)
        return val
    def LineScroll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_LineScroll,(self,) + _args, _kwargs)
        return val
    def EnsureCaretVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EnsureCaretVisible,(self,) + _args, _kwargs)
        return val
    def ReplaceSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ReplaceSelection,(self,) + _args, _kwargs)
        return val
    def SetReadOnly(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetReadOnly,(self,) + _args, _kwargs)
        return val
    def CanPaste(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanPaste,(self,) + _args, _kwargs)
        return val
    def CanUndo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanUndo,(self,) + _args, _kwargs)
        return val
    def EmptyUndoBuffer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EmptyUndoBuffer,(self,) + _args, _kwargs)
        return val
    def Undo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Undo,(self,) + _args, _kwargs)
        return val
    def Cut(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Cut,(self,) + _args, _kwargs)
        return val
    def Copy(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Copy,(self,) + _args, _kwargs)
        return val
    def Paste(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Paste,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Clear,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetText,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetText,(self,) + _args, _kwargs)
        return val
    def GetTextLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetTextLength,(self,) + _args, _kwargs)
        return val
    def SetOvertype(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetOvertype,(self,) + _args, _kwargs)
        return val
    def GetOvertype(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetOvertype,(self,) + _args, _kwargs)
        return val
    def CallTipShow(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipShow,(self,) + _args, _kwargs)
        return val
    def CallTipCancel(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipCancel,(self,) + _args, _kwargs)
        return val
    def CallTipActive(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipActive,(self,) + _args, _kwargs)
        return val
    def CallTipPosAtStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipPosAtStart,(self,) + _args, _kwargs)
        return val
    def CallTipSetHighlight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipSetHighlight,(self,) + _args, _kwargs)
        return val
    def CallTipSetBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CallTipSetBackground,(self,) + _args, _kwargs)
        return val
    def VisibleFromDocLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_VisibleFromDocLine,(self,) + _args, _kwargs)
        return val
    def DocLineFromVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_DocLineFromVisible,(self,) + _args, _kwargs)
        return val
    def SetFoldLevel(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetFoldLevel,(self,) + _args, _kwargs)
        return val
    def GetFoldLevel(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetFoldLevel,(self,) + _args, _kwargs)
        return val
    def GetLastChild(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLastChild,(self,) + _args, _kwargs)
        return val
    def GetFoldParent(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetFoldParent,(self,) + _args, _kwargs)
        return val
    def ShowLines(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ShowLines,(self,) + _args, _kwargs)
        return val
    def HideLines(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_HideLines,(self,) + _args, _kwargs)
        return val
    def GetLineVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineVisible,(self,) + _args, _kwargs)
        return val
    def SetFoldExpanded(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetFoldExpanded,(self,) + _args, _kwargs)
        return val
    def GetFoldExpanded(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetFoldExpanded,(self,) + _args, _kwargs)
        return val
    def ToggleFold(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ToggleFold,(self,) + _args, _kwargs)
        return val
    def EnsureVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EnsureVisible,(self,) + _args, _kwargs)
        return val
    def SetFoldFlags(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetFoldFlags,(self,) + _args, _kwargs)
        return val
    def LineLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_LineLength,(self,) + _args, _kwargs)
        return val
    def BraceHighlight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceHighlight,(self,) + _args, _kwargs)
        return val
    def BraceBadLight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceBadLight,(self,) + _args, _kwargs)
        return val
    def BraceMatch(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceMatch,(self,) + _args, _kwargs)
        return val
    def GetViewEOL(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetViewEOL,(self,) + _args, _kwargs)
        return val
    def SetViewEOL(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetViewEOL,(self,) + _args, _kwargs)
        return val
    def GetDocPointer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetDocPointer,(self,) + _args, _kwargs)
        return val
    def SetDocPointer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetDocPointer,(self,) + _args, _kwargs)
        return val
    def SetModEventMask(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetModEventMask,(self,) + _args, _kwargs)
        return val
    def GetEdgeColumn(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEdgeColumn,(self,) + _args, _kwargs)
        return val
    def SetEdgeColumn(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetEdgeColumn,(self,) + _args, _kwargs)
        return val
    def GetEdgeMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEdgeMode,(self,) + _args, _kwargs)
        return val
    def SetEdgeMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetEdgeMode,(self,) + _args, _kwargs)
        return val
    def GetEdgeColour(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEdgeColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetEdgeColour(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetEdgeColour,(self,) + _args, _kwargs)
        return val
    def SearchAnchor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SearchAnchor,(self,) + _args, _kwargs)
        return val
    def SearchNext(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SearchNext,(self,) + _args, _kwargs)
        return val
    def SearchPrev(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SearchPrev,(self,) + _args, _kwargs)
        return val
    def SetCaretPolicy(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretPolicy,(self,) + _args, _kwargs)
        return val
    def LinesOnScreen(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_LinesOnScreen,(self,) + _args, _kwargs)
        return val
    def UsePopUp(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_UsePopUp,(self,) + _args, _kwargs)
        return val
    def SelectionIsRectangle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SelectionIsRectangle,(self,) + _args, _kwargs)
        return val
    def SetZoom(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetZoom,(self,) + _args, _kwargs)
        return val
    def GetZoom(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetZoom,(self,) + _args, _kwargs)
        return val
    def CreateDocument(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CreateDocument,(self,) + _args, _kwargs)
        return val
    def AddRefDocument(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AddRefDocument,(self,) + _args, _kwargs)
        return val
    def ReleaseDocument(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ReleaseDocument,(self,) + _args, _kwargs)
        return val
    def GetModEventMask(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetModEventMask,(self,) + _args, _kwargs)
        return val
    def StartRecord(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StartRecord,(self,) + _args, _kwargs)
        return val
    def StopRecord(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StopRecord,(self,) + _args, _kwargs)
        return val
    def SetLexer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetLexer,(self,) + _args, _kwargs)
        return val
    def GetLexer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLexer,(self,) + _args, _kwargs)
        return val
    def Colourise(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Colourise,(self,) + _args, _kwargs)
        return val
    def SetProperty(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetProperty,(self,) + _args, _kwargs)
        return val
    def SetKeyWords(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetKeyWords,(self,) + _args, _kwargs)
        return val
    def GetCurrentLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurrentLine,(self,) + _args, _kwargs)
        return val
    def StyleSetSpec(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetSpec,(self,) + _args, _kwargs)
        return val
    def StyleSetFont(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFont,(self,) + _args, _kwargs)
        return val
    def StyleSetFontAttr(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFontAttr,(self,) + _args, _kwargs)
        return val
    def CmdKeyExecute(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyExecute,(self,) + _args, _kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMargins,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelection,(self,) + _args, _kwargs)
        return val
    def PointFromPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PointFromPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ScrollToLine,(self,) + _args, _kwargs)
        return val
    def ScrollToColumn(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ScrollToColumn,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStyledTextCtrl instance at %s>" % (self.this,)
class wxStyledTextCtrl(wxStyledTextCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stc_c.new_wxStyledTextCtrl,_args,_kwargs)
        self.thisown = 1




class wxStyledTextEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,stc_c=stc_c):
        if self.thisown == 1 :
            stc_c.delete_wxStyledTextEvent(self)
    def SetPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetPosition,(self,) + _args, _kwargs)
        return val
    def SetKey(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetKey,(self,) + _args, _kwargs)
        return val
    def SetModifiers(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetModifiers,(self,) + _args, _kwargs)
        return val
    def SetModificationType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetModificationType,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetText,(self,) + _args, _kwargs)
        return val
    def SetLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetLength,(self,) + _args, _kwargs)
        return val
    def SetLinesAdded(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetLinesAdded,(self,) + _args, _kwargs)
        return val
    def SetLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetLine,(self,) + _args, _kwargs)
        return val
    def SetFoldLevelNow(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetFoldLevelNow,(self,) + _args, _kwargs)
        return val
    def SetFoldLevelPrev(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetFoldLevelPrev,(self,) + _args, _kwargs)
        return val
    def SetMargin(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetMargin,(self,) + _args, _kwargs)
        return val
    def SetMessage(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetMessage,(self,) + _args, _kwargs)
        return val
    def SetWParam(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetWParam,(self,) + _args, _kwargs)
        return val
    def SetLParam(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_SetLParam,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetPosition,(self,) + _args, _kwargs)
        return val
    def GetKey(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetKey,(self,) + _args, _kwargs)
        return val
    def GetModifiers(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetModifiers,(self,) + _args, _kwargs)
        return val
    def GetModificationType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetModificationType,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetText,(self,) + _args, _kwargs)
        return val
    def GetLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetLength,(self,) + _args, _kwargs)
        return val
    def GetLinesAdded(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetLinesAdded,(self,) + _args, _kwargs)
        return val
    def GetLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetLine,(self,) + _args, _kwargs)
        return val
    def GetFoldLevelNow(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetFoldLevelNow,(self,) + _args, _kwargs)
        return val
    def GetFoldLevelPrev(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetFoldLevelPrev,(self,) + _args, _kwargs)
        return val
    def GetMargin(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetMargin,(self,) + _args, _kwargs)
        return val
    def GetMessage(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetMessage,(self,) + _args, _kwargs)
        return val
    def GetWParam(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetWParam,(self,) + _args, _kwargs)
        return val
    def GetLParam(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetLParam,(self,) + _args, _kwargs)
        return val
    def GetShift(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetShift,(self,) + _args, _kwargs)
        return val
    def GetControl(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetControl,(self,) + _args, _kwargs)
        return val
    def GetAlt(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_GetAlt,(self,) + _args, _kwargs)
        return val
    def CopyObject(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextEvent_CopyObject,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStyledTextEvent instance at %s>" % (self.this,)
class wxStyledTextEvent(wxStyledTextEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stc_c.new_wxStyledTextEvent,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxSTC_INVALID_POSITION = stc_c.wxSTC_INVALID_POSITION
wxSTC_START = stc_c.wxSTC_START
wxSTC_OPTIONAL_START = stc_c.wxSTC_OPTIONAL_START
wxSTC_LEXER_START = stc_c.wxSTC_LEXER_START
wxSTC_CMD_REDO = stc_c.wxSTC_CMD_REDO
wxSTC_CMD_SELECTALL = stc_c.wxSTC_CMD_SELECTALL
wxSTC_WS_INVISIBLE = stc_c.wxSTC_WS_INVISIBLE
wxSTC_WS_VISIBLEALWAYS = stc_c.wxSTC_WS_VISIBLEALWAYS
wxSTC_WS_VISIBLEAFTERINDENT = stc_c.wxSTC_WS_VISIBLEAFTERINDENT
wxSTC_EOL_CRLF = stc_c.wxSTC_EOL_CRLF
wxSTC_EOL_CR = stc_c.wxSTC_EOL_CR
wxSTC_EOL_LF = stc_c.wxSTC_EOL_LF
wxSTC_CP_UTF8 = stc_c.wxSTC_CP_UTF8
wxSTC_MARKER_MAX = stc_c.wxSTC_MARKER_MAX
wxSTC_MARK_CIRCLE = stc_c.wxSTC_MARK_CIRCLE
wxSTC_MARK_ROUNDRECT = stc_c.wxSTC_MARK_ROUNDRECT
wxSTC_MARK_ARROW = stc_c.wxSTC_MARK_ARROW
wxSTC_MARK_SMALLRECT = stc_c.wxSTC_MARK_SMALLRECT
wxSTC_MARK_SHORTARROW = stc_c.wxSTC_MARK_SHORTARROW
wxSTC_MARK_EMPTY = stc_c.wxSTC_MARK_EMPTY
wxSTC_MARK_ARROWDOWN = stc_c.wxSTC_MARK_ARROWDOWN
wxSTC_MARK_MINUS = stc_c.wxSTC_MARK_MINUS
wxSTC_MARK_PLUS = stc_c.wxSTC_MARK_PLUS
wxSTC_MARKNUM_FOLDER = stc_c.wxSTC_MARKNUM_FOLDER
wxSTC_MARKNUM_FOLDEROPEN = stc_c.wxSTC_MARKNUM_FOLDEROPEN
wxSTC_MARGIN_SYMBOL = stc_c.wxSTC_MARGIN_SYMBOL
wxSTC_MARGIN_NUMBER = stc_c.wxSTC_MARGIN_NUMBER
wxSTC_STYLE_DEFAULT = stc_c.wxSTC_STYLE_DEFAULT
wxSTC_STYLE_LINENUMBER = stc_c.wxSTC_STYLE_LINENUMBER
wxSTC_STYLE_BRACELIGHT = stc_c.wxSTC_STYLE_BRACELIGHT
wxSTC_STYLE_BRACEBAD = stc_c.wxSTC_STYLE_BRACEBAD
wxSTC_STYLE_CONTROLCHAR = stc_c.wxSTC_STYLE_CONTROLCHAR
wxSTC_STYLE_INDENTGUIDE = stc_c.wxSTC_STYLE_INDENTGUIDE
wxSTC_STYLE_MAX = stc_c.wxSTC_STYLE_MAX
wxSTC_CHARSET_ANSI = stc_c.wxSTC_CHARSET_ANSI
wxSTC_CHARSET_DEFAULT = stc_c.wxSTC_CHARSET_DEFAULT
wxSTC_CHARSET_BALTIC = stc_c.wxSTC_CHARSET_BALTIC
wxSTC_CHARSET_CHINESEBIG5 = stc_c.wxSTC_CHARSET_CHINESEBIG5
wxSTC_CHARSET_EASTEUROPE = stc_c.wxSTC_CHARSET_EASTEUROPE
wxSTC_CHARSET_GB2312 = stc_c.wxSTC_CHARSET_GB2312
wxSTC_CHARSET_GREEK = stc_c.wxSTC_CHARSET_GREEK
wxSTC_CHARSET_HANGUL = stc_c.wxSTC_CHARSET_HANGUL
wxSTC_CHARSET_MAC = stc_c.wxSTC_CHARSET_MAC
wxSTC_CHARSET_OEM = stc_c.wxSTC_CHARSET_OEM
wxSTC_CHARSET_RUSSIAN = stc_c.wxSTC_CHARSET_RUSSIAN
wxSTC_CHARSET_SHIFTJIS = stc_c.wxSTC_CHARSET_SHIFTJIS
wxSTC_CHARSET_SYMBOL = stc_c.wxSTC_CHARSET_SYMBOL
wxSTC_CHARSET_TURKISH = stc_c.wxSTC_CHARSET_TURKISH
wxSTC_CHARSET_JOHAB = stc_c.wxSTC_CHARSET_JOHAB
wxSTC_CHARSET_HEBREW = stc_c.wxSTC_CHARSET_HEBREW
wxSTC_CHARSET_ARABIC = stc_c.wxSTC_CHARSET_ARABIC
wxSTC_CHARSET_VIETNAMESE = stc_c.wxSTC_CHARSET_VIETNAMESE
wxSTC_CHARSET_THAI = stc_c.wxSTC_CHARSET_THAI
wxSTC_INDIC_MAX = stc_c.wxSTC_INDIC_MAX
wxSTC_INDIC_PLAIN = stc_c.wxSTC_INDIC_PLAIN
wxSTC_INDIC_SQUIGGLE = stc_c.wxSTC_INDIC_SQUIGGLE
wxSTC_INDIC_TT = stc_c.wxSTC_INDIC_TT
wxSTC_INDIC_DIAGONAL = stc_c.wxSTC_INDIC_DIAGONAL
wxSTC_INDIC_STRIKE = stc_c.wxSTC_INDIC_STRIKE
wxSTC_INDIC0_MASK = stc_c.wxSTC_INDIC0_MASK
wxSTC_INDIC1_MASK = stc_c.wxSTC_INDIC1_MASK
wxSTC_INDIC2_MASK = stc_c.wxSTC_INDIC2_MASK
wxSTC_INDICS_MASK = stc_c.wxSTC_INDICS_MASK
wxSTC_PRINT_NORMAL = stc_c.wxSTC_PRINT_NORMAL
wxSTC_PRINT_INVERTLIGHT = stc_c.wxSTC_PRINT_INVERTLIGHT
wxSTC_PRINT_BLACKONWHITE = stc_c.wxSTC_PRINT_BLACKONWHITE
wxSTC_FIND_DOWN = stc_c.wxSTC_FIND_DOWN
wxSTC_FIND_WHOLEWORD = stc_c.wxSTC_FIND_WHOLEWORD
wxSTC_FIND_MATCHCASE = stc_c.wxSTC_FIND_MATCHCASE
wxSTC_FIND_WORDSTART = stc_c.wxSTC_FIND_WORDSTART
wxSTC_FIND_REGEXP = stc_c.wxSTC_FIND_REGEXP
wxSTC_CMD_UNDO = stc_c.wxSTC_CMD_UNDO
wxSTC_CMD_CUT = stc_c.wxSTC_CMD_CUT
wxSTC_CMD_COPY = stc_c.wxSTC_CMD_COPY
wxSTC_CMD_PASTE = stc_c.wxSTC_CMD_PASTE
wxSTC_FOLDLEVELBASE = stc_c.wxSTC_FOLDLEVELBASE
wxSTC_FOLDLEVELWHITEFLAG = stc_c.wxSTC_FOLDLEVELWHITEFLAG
wxSTC_FOLDLEVELHEADERFLAG = stc_c.wxSTC_FOLDLEVELHEADERFLAG
wxSTC_FOLDLEVELNUMBERMASK = stc_c.wxSTC_FOLDLEVELNUMBERMASK
wxSTC_CMD_LINEDOWN = stc_c.wxSTC_CMD_LINEDOWN
wxSTC_CMD_LINEDOWNEXTEND = stc_c.wxSTC_CMD_LINEDOWNEXTEND
wxSTC_CMD_LINEUP = stc_c.wxSTC_CMD_LINEUP
wxSTC_CMD_LINEUPEXTEND = stc_c.wxSTC_CMD_LINEUPEXTEND
wxSTC_CMD_CHARLEFT = stc_c.wxSTC_CMD_CHARLEFT
wxSTC_CMD_CHARLEFTEXTEND = stc_c.wxSTC_CMD_CHARLEFTEXTEND
wxSTC_CMD_CHARRIGHT = stc_c.wxSTC_CMD_CHARRIGHT
wxSTC_CMD_CHARRIGHTEXTEND = stc_c.wxSTC_CMD_CHARRIGHTEXTEND
wxSTC_CMD_WORDLEFT = stc_c.wxSTC_CMD_WORDLEFT
wxSTC_CMD_WORDLEFTEXTEND = stc_c.wxSTC_CMD_WORDLEFTEXTEND
wxSTC_CMD_WORDRIGHT = stc_c.wxSTC_CMD_WORDRIGHT
wxSTC_CMD_WORDRIGHTEXTEND = stc_c.wxSTC_CMD_WORDRIGHTEXTEND
wxSTC_CMD_HOME = stc_c.wxSTC_CMD_HOME
wxSTC_CMD_HOMEEXTEND = stc_c.wxSTC_CMD_HOMEEXTEND
wxSTC_CMD_LINEEND = stc_c.wxSTC_CMD_LINEEND
wxSTC_CMD_LINEENDEXTEND = stc_c.wxSTC_CMD_LINEENDEXTEND
wxSTC_CMD_DOCUMENTSTART = stc_c.wxSTC_CMD_DOCUMENTSTART
wxSTC_CMD_DOCUMENTSTARTEXTEND = stc_c.wxSTC_CMD_DOCUMENTSTARTEXTEND
wxSTC_CMD_DOCUMENTEND = stc_c.wxSTC_CMD_DOCUMENTEND
wxSTC_CMD_DOCUMENTENDEXTEND = stc_c.wxSTC_CMD_DOCUMENTENDEXTEND
wxSTC_CMD_PAGEUP = stc_c.wxSTC_CMD_PAGEUP
wxSTC_CMD_PAGEUPEXTEND = stc_c.wxSTC_CMD_PAGEUPEXTEND
wxSTC_CMD_PAGEDOWN = stc_c.wxSTC_CMD_PAGEDOWN
wxSTC_CMD_PAGEDOWNEXTEND = stc_c.wxSTC_CMD_PAGEDOWNEXTEND
wxSTC_CMD_EDITTOGGLEOVERTYPE = stc_c.wxSTC_CMD_EDITTOGGLEOVERTYPE
wxSTC_CMD_CANCEL = stc_c.wxSTC_CMD_CANCEL
wxSTC_CMD_DELETEBACK = stc_c.wxSTC_CMD_DELETEBACK
wxSTC_CMD_TAB = stc_c.wxSTC_CMD_TAB
wxSTC_CMD_BACKTAB = stc_c.wxSTC_CMD_BACKTAB
wxSTC_CMD_NEWLINE = stc_c.wxSTC_CMD_NEWLINE
wxSTC_CMD_FORMFEED = stc_c.wxSTC_CMD_FORMFEED
wxSTC_CMD_VCHOME = stc_c.wxSTC_CMD_VCHOME
wxSTC_CMD_VCHOMEEXTEND = stc_c.wxSTC_CMD_VCHOMEEXTEND
wxSTC_CMD_ZOOMIN = stc_c.wxSTC_CMD_ZOOMIN
wxSTC_CMD_ZOOMOUT = stc_c.wxSTC_CMD_ZOOMOUT
wxSTC_CMD_DELWORDLEFT = stc_c.wxSTC_CMD_DELWORDLEFT
wxSTC_CMD_DELWORDRIGHT = stc_c.wxSTC_CMD_DELWORDRIGHT
wxSTC_CMD_LINECUT = stc_c.wxSTC_CMD_LINECUT
wxSTC_CMD_LINEDELETE = stc_c.wxSTC_CMD_LINEDELETE
wxSTC_CMD_LINETRANSPOSE = stc_c.wxSTC_CMD_LINETRANSPOSE
wxSTC_CMD_LOWERCASE = stc_c.wxSTC_CMD_LOWERCASE
wxSTC_CMD_UPPERCASE = stc_c.wxSTC_CMD_UPPERCASE
wxSTC_CMD_LINESCROLLDOWN = stc_c.wxSTC_CMD_LINESCROLLDOWN
wxSTC_CMD_LINESCROLLUP = stc_c.wxSTC_CMD_LINESCROLLUP
wxSTC_EDGE_NONE = stc_c.wxSTC_EDGE_NONE
wxSTC_EDGE_LINE = stc_c.wxSTC_EDGE_LINE
wxSTC_EDGE_BACKGROUND = stc_c.wxSTC_EDGE_BACKGROUND
wxSTC_CARET_SLOP = stc_c.wxSTC_CARET_SLOP
wxSTC_CARET_CENTER = stc_c.wxSTC_CARET_CENTER
wxSTC_CARET_STRICT = stc_c.wxSTC_CARET_STRICT
wxSTC_MOD_INSERTTEXT = stc_c.wxSTC_MOD_INSERTTEXT
wxSTC_MOD_DELETETEXT = stc_c.wxSTC_MOD_DELETETEXT
wxSTC_MOD_CHANGESTYLE = stc_c.wxSTC_MOD_CHANGESTYLE
wxSTC_MOD_CHANGEFOLD = stc_c.wxSTC_MOD_CHANGEFOLD
wxSTC_PERFORMED_USER = stc_c.wxSTC_PERFORMED_USER
wxSTC_PERFORMED_UNDO = stc_c.wxSTC_PERFORMED_UNDO
wxSTC_PERFORMED_REDO = stc_c.wxSTC_PERFORMED_REDO
wxSTC_LASTSTEPINUNDOREDO = stc_c.wxSTC_LASTSTEPINUNDOREDO
wxSTC_MOD_CHANGEMARKER = stc_c.wxSTC_MOD_CHANGEMARKER
wxSTC_MOD_BEFOREINSERT = stc_c.wxSTC_MOD_BEFOREINSERT
wxSTC_MOD_BEFOREDELETE = stc_c.wxSTC_MOD_BEFOREDELETE
wxSTC_MODEVENTMASKALL = stc_c.wxSTC_MODEVENTMASKALL
wxSTC_KEY_DOWN = stc_c.wxSTC_KEY_DOWN
wxSTC_KEY_UP = stc_c.wxSTC_KEY_UP
wxSTC_KEY_LEFT = stc_c.wxSTC_KEY_LEFT
wxSTC_KEY_RIGHT = stc_c.wxSTC_KEY_RIGHT
wxSTC_KEY_HOME = stc_c.wxSTC_KEY_HOME
wxSTC_KEY_END = stc_c.wxSTC_KEY_END
wxSTC_KEY_PRIOR = stc_c.wxSTC_KEY_PRIOR
wxSTC_KEY_NEXT = stc_c.wxSTC_KEY_NEXT
wxSTC_KEY_DELETE = stc_c.wxSTC_KEY_DELETE
wxSTC_KEY_INSERT = stc_c.wxSTC_KEY_INSERT
wxSTC_KEY_ESCAPE = stc_c.wxSTC_KEY_ESCAPE
wxSTC_KEY_BACK = stc_c.wxSTC_KEY_BACK
wxSTC_KEY_TAB = stc_c.wxSTC_KEY_TAB
wxSTC_KEY_RETURN = stc_c.wxSTC_KEY_RETURN
wxSTC_KEY_ADD = stc_c.wxSTC_KEY_ADD
wxSTC_KEY_SUBTRACT = stc_c.wxSTC_KEY_SUBTRACT
wxSTC_KEY_DIVIDE = stc_c.wxSTC_KEY_DIVIDE
wxSTC_SCMOD_SHIFT = stc_c.wxSTC_SCMOD_SHIFT
wxSTC_SCMOD_CTRL = stc_c.wxSTC_SCMOD_CTRL
wxSTC_SCMOD_ALT = stc_c.wxSTC_SCMOD_ALT
wxSTC_LEX_CONTAINER = stc_c.wxSTC_LEX_CONTAINER
wxSTC_LEX_NULL = stc_c.wxSTC_LEX_NULL
wxSTC_LEX_PYTHON = stc_c.wxSTC_LEX_PYTHON
wxSTC_LEX_CPP = stc_c.wxSTC_LEX_CPP
wxSTC_LEX_HTML = stc_c.wxSTC_LEX_HTML
wxSTC_LEX_XML = stc_c.wxSTC_LEX_XML
wxSTC_LEX_PERL = stc_c.wxSTC_LEX_PERL
wxSTC_LEX_SQL = stc_c.wxSTC_LEX_SQL
wxSTC_LEX_VB = stc_c.wxSTC_LEX_VB
wxSTC_LEX_PROPERTIES = stc_c.wxSTC_LEX_PROPERTIES
wxSTC_LEX_ERRORLIST = stc_c.wxSTC_LEX_ERRORLIST
wxSTC_LEX_MAKEFILE = stc_c.wxSTC_LEX_MAKEFILE
wxSTC_LEX_BATCH = stc_c.wxSTC_LEX_BATCH
wxSTC_LEX_XCODE = stc_c.wxSTC_LEX_XCODE
wxSTC_LEX_LATEX = stc_c.wxSTC_LEX_LATEX
wxSTC_LEX_LUA = stc_c.wxSTC_LEX_LUA
wxSTC_LEX_DIFF = stc_c.wxSTC_LEX_DIFF
wxSTC_P_DEFAULT = stc_c.wxSTC_P_DEFAULT
wxSTC_P_COMMENTLINE = stc_c.wxSTC_P_COMMENTLINE
wxSTC_P_NUMBER = stc_c.wxSTC_P_NUMBER
wxSTC_P_STRING = stc_c.wxSTC_P_STRING
wxSTC_P_CHARACTER = stc_c.wxSTC_P_CHARACTER
wxSTC_P_WORD = stc_c.wxSTC_P_WORD
wxSTC_P_TRIPLE = stc_c.wxSTC_P_TRIPLE
wxSTC_P_TRIPLEDOUBLE = stc_c.wxSTC_P_TRIPLEDOUBLE
wxSTC_P_CLASSNAME = stc_c.wxSTC_P_CLASSNAME
wxSTC_P_DEFNAME = stc_c.wxSTC_P_DEFNAME
wxSTC_P_OPERATOR = stc_c.wxSTC_P_OPERATOR
wxSTC_P_IDENTIFIER = stc_c.wxSTC_P_IDENTIFIER
wxSTC_P_COMMENTBLOCK = stc_c.wxSTC_P_COMMENTBLOCK
wxSTC_P_STRINGEOL = stc_c.wxSTC_P_STRINGEOL
wxSTC_C_DEFAULT = stc_c.wxSTC_C_DEFAULT
wxSTC_C_COMMENT = stc_c.wxSTC_C_COMMENT
wxSTC_C_COMMENTLINE = stc_c.wxSTC_C_COMMENTLINE
wxSTC_C_COMMENTDOC = stc_c.wxSTC_C_COMMENTDOC
wxSTC_C_NUMBER = stc_c.wxSTC_C_NUMBER
wxSTC_C_WORD = stc_c.wxSTC_C_WORD
wxSTC_C_STRING = stc_c.wxSTC_C_STRING
wxSTC_C_CHARACTER = stc_c.wxSTC_C_CHARACTER
wxSTC_C_UUID = stc_c.wxSTC_C_UUID
wxSTC_C_PREPROCESSOR = stc_c.wxSTC_C_PREPROCESSOR
wxSTC_C_OPERATOR = stc_c.wxSTC_C_OPERATOR
wxSTC_C_IDENTIFIER = stc_c.wxSTC_C_IDENTIFIER
wxSTC_C_STRINGEOL = stc_c.wxSTC_C_STRINGEOL
wxSTC_C_VERBATIM = stc_c.wxSTC_C_VERBATIM
wxSTC_H_DEFAULT = stc_c.wxSTC_H_DEFAULT
wxSTC_H_TAG = stc_c.wxSTC_H_TAG
wxSTC_H_TAGUNKNOWN = stc_c.wxSTC_H_TAGUNKNOWN
wxSTC_H_ATTRIBUTE = stc_c.wxSTC_H_ATTRIBUTE
wxSTC_H_ATTRIBUTEUNKNOWN = stc_c.wxSTC_H_ATTRIBUTEUNKNOWN
wxSTC_H_NUMBER = stc_c.wxSTC_H_NUMBER
wxSTC_H_DOUBLESTRING = stc_c.wxSTC_H_DOUBLESTRING
wxSTC_H_SINGLESTRING = stc_c.wxSTC_H_SINGLESTRING
wxSTC_H_OTHER = stc_c.wxSTC_H_OTHER
wxSTC_H_COMMENT = stc_c.wxSTC_H_COMMENT
wxSTC_H_ENTITY = stc_c.wxSTC_H_ENTITY
wxSTC_H_TAGEND = stc_c.wxSTC_H_TAGEND
wxSTC_H_XMLSTART = stc_c.wxSTC_H_XMLSTART
wxSTC_H_XMLEND = stc_c.wxSTC_H_XMLEND
wxSTC_H_SCRIPT = stc_c.wxSTC_H_SCRIPT
wxSTC_H_ASP = stc_c.wxSTC_H_ASP
wxSTC_H_ASPAT = stc_c.wxSTC_H_ASPAT
wxSTC_H_CDATA = stc_c.wxSTC_H_CDATA
wxSTC_H_QUESTION = stc_c.wxSTC_H_QUESTION
wxSTC_H_VALUE = stc_c.wxSTC_H_VALUE
wxSTC_HJ_START = stc_c.wxSTC_HJ_START
wxSTC_HJ_DEFAULT = stc_c.wxSTC_HJ_DEFAULT
wxSTC_HJ_COMMENT = stc_c.wxSTC_HJ_COMMENT
wxSTC_HJ_COMMENTLINE = stc_c.wxSTC_HJ_COMMENTLINE
wxSTC_HJ_COMMENTDOC = stc_c.wxSTC_HJ_COMMENTDOC
wxSTC_HJ_NUMBER = stc_c.wxSTC_HJ_NUMBER
wxSTC_HJ_WORD = stc_c.wxSTC_HJ_WORD
wxSTC_HJ_KEYWORD = stc_c.wxSTC_HJ_KEYWORD
wxSTC_HJ_DOUBLESTRING = stc_c.wxSTC_HJ_DOUBLESTRING
wxSTC_HJ_SINGLESTRING = stc_c.wxSTC_HJ_SINGLESTRING
wxSTC_HJ_SYMBOLS = stc_c.wxSTC_HJ_SYMBOLS
wxSTC_HJ_STRINGEOL = stc_c.wxSTC_HJ_STRINGEOL
wxSTC_HJA_START = stc_c.wxSTC_HJA_START
wxSTC_HJA_DEFAULT = stc_c.wxSTC_HJA_DEFAULT
wxSTC_HJA_COMMENT = stc_c.wxSTC_HJA_COMMENT
wxSTC_HJA_COMMENTLINE = stc_c.wxSTC_HJA_COMMENTLINE
wxSTC_HJA_COMMENTDOC = stc_c.wxSTC_HJA_COMMENTDOC
wxSTC_HJA_NUMBER = stc_c.wxSTC_HJA_NUMBER
wxSTC_HJA_WORD = stc_c.wxSTC_HJA_WORD
wxSTC_HJA_KEYWORD = stc_c.wxSTC_HJA_KEYWORD
wxSTC_HJA_DOUBLESTRING = stc_c.wxSTC_HJA_DOUBLESTRING
wxSTC_HJA_SINGLESTRING = stc_c.wxSTC_HJA_SINGLESTRING
wxSTC_HJA_SYMBOLS = stc_c.wxSTC_HJA_SYMBOLS
wxSTC_HJA_STRINGEOL = stc_c.wxSTC_HJA_STRINGEOL
wxSTC_HB_START = stc_c.wxSTC_HB_START
wxSTC_HB_DEFAULT = stc_c.wxSTC_HB_DEFAULT
wxSTC_HB_COMMENTLINE = stc_c.wxSTC_HB_COMMENTLINE
wxSTC_HB_NUMBER = stc_c.wxSTC_HB_NUMBER
wxSTC_HB_WORD = stc_c.wxSTC_HB_WORD
wxSTC_HB_STRING = stc_c.wxSTC_HB_STRING
wxSTC_HB_IDENTIFIER = stc_c.wxSTC_HB_IDENTIFIER
wxSTC_HB_STRINGEOL = stc_c.wxSTC_HB_STRINGEOL
wxSTC_HBA_START = stc_c.wxSTC_HBA_START
wxSTC_HBA_DEFAULT = stc_c.wxSTC_HBA_DEFAULT
wxSTC_HBA_COMMENTLINE = stc_c.wxSTC_HBA_COMMENTLINE
wxSTC_HBA_NUMBER = stc_c.wxSTC_HBA_NUMBER
wxSTC_HBA_WORD = stc_c.wxSTC_HBA_WORD
wxSTC_HBA_STRING = stc_c.wxSTC_HBA_STRING
wxSTC_HBA_IDENTIFIER = stc_c.wxSTC_HBA_IDENTIFIER
wxSTC_HBA_STRINGEOL = stc_c.wxSTC_HBA_STRINGEOL
wxSTC_HP_START = stc_c.wxSTC_HP_START
wxSTC_HP_DEFAULT = stc_c.wxSTC_HP_DEFAULT
wxSTC_HP_COMMENTLINE = stc_c.wxSTC_HP_COMMENTLINE
wxSTC_HP_NUMBER = stc_c.wxSTC_HP_NUMBER
wxSTC_HP_STRING = stc_c.wxSTC_HP_STRING
wxSTC_HP_CHARACTER = stc_c.wxSTC_HP_CHARACTER
wxSTC_HP_WORD = stc_c.wxSTC_HP_WORD
wxSTC_HP_TRIPLE = stc_c.wxSTC_HP_TRIPLE
wxSTC_HP_TRIPLEDOUBLE = stc_c.wxSTC_HP_TRIPLEDOUBLE
wxSTC_HP_CLASSNAME = stc_c.wxSTC_HP_CLASSNAME
wxSTC_HP_DEFNAME = stc_c.wxSTC_HP_DEFNAME
wxSTC_HP_OPERATOR = stc_c.wxSTC_HP_OPERATOR
wxSTC_HP_IDENTIFIER = stc_c.wxSTC_HP_IDENTIFIER
wxSTC_HPA_START = stc_c.wxSTC_HPA_START
wxSTC_HPA_DEFAULT = stc_c.wxSTC_HPA_DEFAULT
wxSTC_HPA_COMMENTLINE = stc_c.wxSTC_HPA_COMMENTLINE
wxSTC_HPA_NUMBER = stc_c.wxSTC_HPA_NUMBER
wxSTC_HPA_STRING = stc_c.wxSTC_HPA_STRING
wxSTC_HPA_CHARACTER = stc_c.wxSTC_HPA_CHARACTER
wxSTC_HPA_WORD = stc_c.wxSTC_HPA_WORD
wxSTC_HPA_TRIPLE = stc_c.wxSTC_HPA_TRIPLE
wxSTC_HPA_TRIPLEDOUBLE = stc_c.wxSTC_HPA_TRIPLEDOUBLE
wxSTC_HPA_CLASSNAME = stc_c.wxSTC_HPA_CLASSNAME
wxSTC_HPA_DEFNAME = stc_c.wxSTC_HPA_DEFNAME
wxSTC_HPA_OPERATOR = stc_c.wxSTC_HPA_OPERATOR
wxSTC_HPA_IDENTIFIER = stc_c.wxSTC_HPA_IDENTIFIER
wxSTC_HPHP_DEFAULT = stc_c.wxSTC_HPHP_DEFAULT
wxSTC_HPHP_HSTRING = stc_c.wxSTC_HPHP_HSTRING
wxSTC_HPHP_SIMPLESTRING = stc_c.wxSTC_HPHP_SIMPLESTRING
wxSTC_HPHP_WORD = stc_c.wxSTC_HPHP_WORD
wxSTC_HPHP_NUMBER = stc_c.wxSTC_HPHP_NUMBER
wxSTC_HPHP_VARIABLE = stc_c.wxSTC_HPHP_VARIABLE
wxSTC_HPHP_COMMENT = stc_c.wxSTC_HPHP_COMMENT
wxSTC_HPHP_COMMENTLINE = stc_c.wxSTC_HPHP_COMMENTLINE
wxSTC_HPHP_STRINGEOL = stc_c.wxSTC_HPHP_STRINGEOL
wxSTC_PL_DEFAULT = stc_c.wxSTC_PL_DEFAULT
wxSTC_PL_HERE = stc_c.wxSTC_PL_HERE
wxSTC_PL_COMMENTLINE = stc_c.wxSTC_PL_COMMENTLINE
wxSTC_PL_POD = stc_c.wxSTC_PL_POD
wxSTC_PL_NUMBER = stc_c.wxSTC_PL_NUMBER
wxSTC_PL_WORD = stc_c.wxSTC_PL_WORD
wxSTC_PL_STRING = stc_c.wxSTC_PL_STRING
wxSTC_PL_CHARACTER = stc_c.wxSTC_PL_CHARACTER
wxSTC_PL_PUNCTUATION = stc_c.wxSTC_PL_PUNCTUATION
wxSTC_PL_PREPROCESSOR = stc_c.wxSTC_PL_PREPROCESSOR
wxSTC_PL_OPERATOR = stc_c.wxSTC_PL_OPERATOR
wxSTC_PL_IDENTIFIER = stc_c.wxSTC_PL_IDENTIFIER
wxSTC_PL_SCALAR = stc_c.wxSTC_PL_SCALAR
wxSTC_PL_ARRAY = stc_c.wxSTC_PL_ARRAY
wxSTC_PL_HASH = stc_c.wxSTC_PL_HASH
wxSTC_PL_SYMBOLTABLE = stc_c.wxSTC_PL_SYMBOLTABLE
wxSTC_PL_REF = stc_c.wxSTC_PL_REF
wxSTC_PL_REGEX = stc_c.wxSTC_PL_REGEX
wxSTC_PL_REGSUBST = stc_c.wxSTC_PL_REGSUBST
wxSTC_PL_LONGQUOTE = stc_c.wxSTC_PL_LONGQUOTE
wxSTC_PL_BACKTICKS = stc_c.wxSTC_PL_BACKTICKS
wxSTC_PL_DATASECTION = stc_c.wxSTC_PL_DATASECTION
wxSTC_L_DEFAULT = stc_c.wxSTC_L_DEFAULT
wxSTC_L_COMMAND = stc_c.wxSTC_L_COMMAND
wxSTC_L_TAG = stc_c.wxSTC_L_TAG
wxSTC_L_MATH = stc_c.wxSTC_L_MATH
wxSTC_L_COMMENT = stc_c.wxSTC_L_COMMENT
wxSTC_LUA_DEFAULT = stc_c.wxSTC_LUA_DEFAULT
wxSTC_LUA_COMMENT = stc_c.wxSTC_LUA_COMMENT
wxSTC_LUA_COMMENTLINE = stc_c.wxSTC_LUA_COMMENTLINE
wxSTC_LUA_COMMENTDOC = stc_c.wxSTC_LUA_COMMENTDOC
wxSTC_LUA_NUMBER = stc_c.wxSTC_LUA_NUMBER
wxSTC_LUA_WORD = stc_c.wxSTC_LUA_WORD
wxSTC_LUA_STRING = stc_c.wxSTC_LUA_STRING
wxSTC_LUA_CHARACTER = stc_c.wxSTC_LUA_CHARACTER
wxSTC_LUA_LITERALSTRING = stc_c.wxSTC_LUA_LITERALSTRING
wxSTC_LUA_PREPROCESSOR = stc_c.wxSTC_LUA_PREPROCESSOR
wxSTC_LUA_OPERATOR = stc_c.wxSTC_LUA_OPERATOR
wxSTC_LUA_IDENTIFIER = stc_c.wxSTC_LUA_IDENTIFIER
wxSTC_LUA_STRINGEOL = stc_c.wxSTC_LUA_STRINGEOL
wxSTC_ERR_DEFAULT = stc_c.wxSTC_ERR_DEFAULT
wxSTC_ERR_PYTHON = stc_c.wxSTC_ERR_PYTHON
wxSTC_ERR_GCC = stc_c.wxSTC_ERR_GCC
wxSTC_ERR_MS = stc_c.wxSTC_ERR_MS
wxSTC_ERR_CMD = stc_c.wxSTC_ERR_CMD
wxSTC_ERR_BORLAND = stc_c.wxSTC_ERR_BORLAND
wxSTC_ERR_PERL = stc_c.wxSTC_ERR_PERL
wxSTC_MASK_FOLDERS = stc_c.wxSTC_MASK_FOLDERS
wxSTCNameStr = stc_c.wxSTCNameStr
wxEVT_STC_CHANGE = stc_c.wxEVT_STC_CHANGE
wxEVT_STC_STYLENEEDED = stc_c.wxEVT_STC_STYLENEEDED
wxEVT_STC_CHARADDED = stc_c.wxEVT_STC_CHARADDED
wxEVT_STC_UPDATEUI = stc_c.wxEVT_STC_UPDATEUI
wxEVT_STC_SAVEPOINTREACHED = stc_c.wxEVT_STC_SAVEPOINTREACHED
wxEVT_STC_SAVEPOINTLEFT = stc_c.wxEVT_STC_SAVEPOINTLEFT
wxEVT_STC_ROMODIFYATTEMPT = stc_c.wxEVT_STC_ROMODIFYATTEMPT
wxEVT_STC_DOUBLECLICK = stc_c.wxEVT_STC_DOUBLECLICK
wxEVT_STC_MODIFIED = stc_c.wxEVT_STC_MODIFIED
wxEVT_STC_KEY = stc_c.wxEVT_STC_KEY
wxEVT_STC_MACRORECORD = stc_c.wxEVT_STC_MACRORECORD
wxEVT_STC_MARGINCLICK = stc_c.wxEVT_STC_MARGINCLICK
wxEVT_STC_NEEDSHOWN = stc_c.wxEVT_STC_NEEDSHOWN
wxEVT_STC_POSCHANGED = stc_c.wxEVT_STC_POSCHANGED


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxStyledTextEventPtr       = wxStyledTextEventPtr
wx.wxStyledTextCtrlPtr        = wxStyledTextCtrlPtr
