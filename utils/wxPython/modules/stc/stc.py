# This file was created automatically by SWIG.
import stcc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from clip_dnd import *

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

def EVT_STC_CMDKEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CMDKEY, fn)

def EVT_STC_UNKNOWNCMDKEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_UNKNOWNCMDKEY, fn)


class wxStyledTextCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetText,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetText,(self,) + _args, _kwargs)
        return val
    def GetLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLine,(self,) + _args, _kwargs)
        return val
    def ReplaceSelection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ReplaceSelection,(self,) + _args, _kwargs)
        return val
    def SetReadOnly(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetReadOnly,(self,) + _args, _kwargs)
        return val
    def GetReadOnly(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetReadOnly,(self,) + _args, _kwargs)
        return val
    def GetTextRange(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetTextRange,(self,) + _args, _kwargs)
        return val
    def GetStyledTextRange(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetStyledTextRange,(self,) + _args, _kwargs)
        return val
    def AddText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AddText,(self,) + _args, _kwargs)
        return val
    def AddStyledText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AddStyledText,(self,) + _args, _kwargs)
        return val
    def InsertText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_InsertText,(self,) + _args, _kwargs)
        return val
    def ClearAll(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ClearAll,(self,) + _args, _kwargs)
        return val
    def GetCharAt(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetCharAt,(self,) + _args, _kwargs)
        return val
    def GetStyleAt(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetStyleAt,(self,) + _args, _kwargs)
        return val
    def SetStyleBits(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetStyleBits,(self,) + _args, _kwargs)
        return val
    def GetStyleBits(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetStyleBits,(self,) + _args, _kwargs)
        return val
    def Cut(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Cut,(self,) + _args, _kwargs)
        return val
    def Copy(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Copy,(self,) + _args, _kwargs)
        return val
    def Paste(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Paste,(self,) + _args, _kwargs)
        return val
    def CanPaste(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CanPaste,(self,) + _args, _kwargs)
        return val
    def ClearClipbrd(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ClearClipbrd,(self,) + _args, _kwargs)
        return val
    def Undo(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Undo,(self,) + _args, _kwargs)
        return val
    def CanUndo(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CanUndo,(self,) + _args, _kwargs)
        return val
    def EmptyUndoBuffer(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_EmptyUndoBuffer,(self,) + _args, _kwargs)
        return val
    def Redo(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Redo,(self,) + _args, _kwargs)
        return val
    def CanRedo(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CanRedo,(self,) + _args, _kwargs)
        return val
    def SetUndoCollection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetUndoCollection,(self,) + _args, _kwargs)
        return val
    def GetUndoCollection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetUndoCollection,(self,) + _args, _kwargs)
        return val
    def BeginUndoAction(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_BeginUndoAction,(self,) + _args, _kwargs)
        return val
    def EndUndoAction(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_EndUndoAction,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetSelection,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetSelection,(self,) + _args, _kwargs)
        return val
    def GetSelectedText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetSelectedText,(self,) + _args, _kwargs)
        return val
    def HideSelection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_HideSelection,(self,) + _args, _kwargs)
        return val
    def GetHideSelection(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetHideSelection,(self,) + _args, _kwargs)
        return val
    def GetTextLength(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetTextLength,(self,) + _args, _kwargs)
        return val
    def GetFirstVisibleLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetFirstVisibleLine,(self,) + _args, _kwargs)
        return val
    def GetModified(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetModified,(self,) + _args, _kwargs)
        return val
    def GetLineCount(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineCount,(self,) + _args, _kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetLineFromPos(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineFromPos,(self,) + _args, _kwargs)
        return val
    def GetLineStartPos(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineStartPos,(self,) + _args, _kwargs)
        return val
    def GetLineLengthAtPos(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineLengthAtPos,(self,) + _args, _kwargs)
        return val
    def GetLineLength(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineLength,(self,) + _args, _kwargs)
        return val
    def GetCurrentLineText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetCurrentLineText,(self,) + _args, _kwargs)
        return val
    def GetCurrentLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetCurrentLine,(self,) + _args, _kwargs)
        return val
    def PositionFromPoint(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_PositionFromPoint,(self,) + _args, _kwargs)
        return val
    def LineFromPoint(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_LineFromPoint,(self,) + _args, _kwargs)
        return val
    def PointFromPosition(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_PointFromPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetCurrentPos(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetCurrentPos,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetAnchor,(self,) + _args, _kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SelectAll,(self,) + _args, _kwargs)
        return val
    def SetCurrentPosition(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetCurrentPosition,(self,) + _args, _kwargs)
        return val
    def SetAnchor(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetAnchor,(self,) + _args, _kwargs)
        return val
    def GotoPos(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GotoPos,(self,) + _args, _kwargs)
        return val
    def GotoLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GotoLine,(self,) + _args, _kwargs)
        return val
    def ChangePosition(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ChangePosition,(self,) + _args, _kwargs)
        return val
    def PageMove(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_PageMove,(self,) + _args, _kwargs)
        return val
    def ScrollBy(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ScrollBy,(self,) + _args, _kwargs)
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ScrollToLine,(self,) + _args, _kwargs)
        return val
    def ScrollToColumn(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ScrollToColumn,(self,) + _args, _kwargs)
        return val
    def EnsureCaretVisible(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_EnsureCaretVisible,(self,) + _args, _kwargs)
        return val
    def SetCaretPolicy(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetCaretPolicy,(self,) + _args, _kwargs)
        return val
    def GetSelectionType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetSelectionType,(self,) + _args, _kwargs)
        return val
    def FindText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_FindText,(self,) + _args, _kwargs)
        return val
    def SearchAnchor(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SearchAnchor,(self,) + _args, _kwargs)
        return val
    def SearchNext(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SearchNext,(self,) + _args, _kwargs)
        return val
    def SearchPrev(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SearchPrev,(self,) + _args, _kwargs)
        return val
    def GetViewWhitespace(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetViewWhitespace,(self,) + _args, _kwargs)
        return val
    def SetViewWhitespace(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetViewWhitespace,(self,) + _args, _kwargs)
        return val
    def GetEOLMode(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetEOLMode,(self,) + _args, _kwargs)
        return val
    def SetEOLMode(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetEOLMode,(self,) + _args, _kwargs)
        return val
    def GetViewEOL(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetViewEOL,(self,) + _args, _kwargs)
        return val
    def SetViewEOL(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetViewEOL,(self,) + _args, _kwargs)
        return val
    def ConvertEOL(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ConvertEOL,(self,) + _args, _kwargs)
        return val
    def GetEndStyled(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetEndStyled,(self,) + _args, _kwargs)
        return val
    def StartStyling(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StartStyling,(self,) + _args, _kwargs)
        return val
    def SetStyleFor(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetStyleFor,(self,) + _args, _kwargs)
        return val
    def SetStyleBytes(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetStyleBytes,(self,) + _args, _kwargs)
        return val
    def StyleClearAll(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleClearAll,(self,) + _args, _kwargs)
        return val
    def StyleResetDefault(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleResetDefault,(self,) + _args, _kwargs)
        return val
    def StyleSetSpec(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetSpec,(self,) + _args, _kwargs)
        return val
    def StyleSetForeground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetForeground,(self,) + _args, _kwargs)
        return val
    def StyleSetBackground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetBackground,(self,) + _args, _kwargs)
        return val
    def StyleSetFont(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetFont,(self,) + _args, _kwargs)
        return val
    def StyleSetFontAttr(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetFontAttr,(self,) + _args, _kwargs)
        return val
    def StyleSetBold(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetBold,(self,) + _args, _kwargs)
        return val
    def StyleSetItalic(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetItalic,(self,) + _args, _kwargs)
        return val
    def StyleSetFaceName(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetFaceName,(self,) + _args, _kwargs)
        return val
    def StyleSetSize(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetSize,(self,) + _args, _kwargs)
        return val
    def StyleSetEOLFilled(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_StyleSetEOLFilled,(self,) + _args, _kwargs)
        return val
    def GetLeftMargin(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLeftMargin,(self,) + _args, _kwargs)
        return val
    def GetRightMargin(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetRightMargin,(self,) + _args, _kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetMargins,(self,) + _args, _kwargs)
        return val
    def SetMarginType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetMarginType,(self,) + _args, _kwargs)
        return val
    def GetMarginType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetMarginType,(self,) + _args, _kwargs)
        return val
    def SetMarginWidth(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetMarginWidth,(self,) + _args, _kwargs)
        return val
    def GetMarginWidth(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetMarginWidth,(self,) + _args, _kwargs)
        return val
    def SetMarginMask(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetMarginMask,(self,) + _args, _kwargs)
        return val
    def GetMarginMask(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetMarginMask,(self,) + _args, _kwargs)
        return val
    def SetMarginSensitive(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetMarginSensitive,(self,) + _args, _kwargs)
        return val
    def GetMarginSensitive(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetMarginSensitive,(self,) + _args, _kwargs)
        return val
    def SetSelectionForeground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetSelectionForeground,(self,) + _args, _kwargs)
        return val
    def SetSelectionBackground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetSelectionBackground,(self,) + _args, _kwargs)
        return val
    def SetCaretForeground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetCaretForeground,(self,) + _args, _kwargs)
        return val
    def GetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetBufferedDraw(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetBufferedDraw,(self,) + _args, _kwargs)
        return val
    def SetTabWidth(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetTabWidth,(self,) + _args, _kwargs)
        return val
    def SetWordChars(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetWordChars,(self,) + _args, _kwargs)
        return val
    def BraceHighlight(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_BraceHighlight,(self,) + _args, _kwargs)
        return val
    def BraceBadlight(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_BraceBadlight,(self,) + _args, _kwargs)
        return val
    def BraceMatch(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_BraceMatch,(self,) + _args, _kwargs)
        return val
    def MarkerDefine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerDefine,(self,) + _args, _kwargs)
        return val
    def MarkerSetType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerSetType,(self,) + _args, _kwargs)
        return val
    def MarkerSetForeground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerSetForeground,(self,) + _args, _kwargs)
        return val
    def MarkerSetBackground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerSetBackground,(self,) + _args, _kwargs)
        return val
    def MarkerAdd(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerAdd,(self,) + _args, _kwargs)
        return val
    def MarkerDelete(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerDelete,(self,) + _args, _kwargs)
        return val
    def MarkerDeleteAll(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerDeleteAll,(self,) + _args, _kwargs)
        return val
    def MarkerGet(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerGet,(self,) + _args, _kwargs)
        return val
    def MarkerGetNextLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerGetNextLine,(self,) + _args, _kwargs)
        return val
    def MarkerGetPrevLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerGetPrevLine,(self,) + _args, _kwargs)
        return val
    def MarkerLineFromHandle(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerLineFromHandle,(self,) + _args, _kwargs)
        return val
    def MarkerDeleteHandle(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_MarkerDeleteHandle,(self,) + _args, _kwargs)
        return val
    def IndicatorSetStyle(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_IndicatorSetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorGetStyle(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_IndicatorGetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorSetColour(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_IndicatorSetColour,(self,) + _args, _kwargs)
        return val
    def AutoCompShow(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompShow,(self,) + _args, _kwargs)
        return val
    def AutoCompCancel(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompCancel,(self,) + _args, _kwargs)
        return val
    def AutoCompActive(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompActive,(self,) + _args, _kwargs)
        return val
    def AutoCompPosAtStart(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompPosAtStart,(self,) + _args, _kwargs)
        return val
    def AutoCompComplete(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompComplete,(self,) + _args, _kwargs)
        return val
    def AutoCompStopChars(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_AutoCompStopChars,(self,) + _args, _kwargs)
        return val
    def CallTipShow(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipShow,(self,) + _args, _kwargs)
        return val
    def CallTipCancel(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipCancel,(self,) + _args, _kwargs)
        return val
    def CallTipActive(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipActive,(self,) + _args, _kwargs)
        return val
    def CallTipPosAtStart(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipPosAtStart,(self,) + _args, _kwargs)
        return val
    def CallTipSetHighlight(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipSetHighlight,(self,) + _args, _kwargs)
        return val
    def CallTipSetBackground(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CallTipSetBackground,(self,) + _args, _kwargs)
        return val
    def CmdKeyAssign(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CmdKeyAssign,(self,) + _args, _kwargs)
        return val
    def CmdKeyClear(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CmdKeyClear,(self,) + _args, _kwargs)
        return val
    def CmdKeyClearAll(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CmdKeyClearAll,(self,) + _args, _kwargs)
        return val
    def CmdKeyExecute(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_CmdKeyExecute,(self,) + _args, _kwargs)
        return val
    def FormatRange(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_FormatRange,(self,) + _args, _kwargs)
        return val
    def GetDocument(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetDocument,(self,) + _args, _kwargs)
        return val
    def SetDocument(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetDocument,(self,) + _args, _kwargs)
        return val
    def VisibleFromDocLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_VisibleFromDocLine,(self,) + _args, _kwargs)
        return val
    def DocLineFromVisible(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_DocLineFromVisible,(self,) + _args, _kwargs)
        return val
    def SetFoldLevel(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetFoldLevel,(self,) + _args, _kwargs)
        return val
    def GetFoldLevel(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetFoldLevel,(self,) + _args, _kwargs)
        return val
    def GetLastChild(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLastChild,(self,) + _args, _kwargs)
        return val
    def GetFoldParent(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetFoldParent,(self,) + _args, _kwargs)
        return val
    def ShowLines(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ShowLines,(self,) + _args, _kwargs)
        return val
    def HideLines(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_HideLines,(self,) + _args, _kwargs)
        return val
    def GetLineVisible(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLineVisible,(self,) + _args, _kwargs)
        return val
    def SetFoldExpanded(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetFoldExpanded,(self,) + _args, _kwargs)
        return val
    def GetFoldExpanded(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetFoldExpanded,(self,) + _args, _kwargs)
        return val
    def ToggleFold(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_ToggleFold,(self,) + _args, _kwargs)
        return val
    def EnsureVisible(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_EnsureVisible,(self,) + _args, _kwargs)
        return val
    def GetEdgeColumn(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetEdgeColumn,(self,) + _args, _kwargs)
        return val
    def SetEdgeColumn(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetEdgeColumn,(self,) + _args, _kwargs)
        return val
    def GetEdgeMode(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetEdgeMode,(self,) + _args, _kwargs)
        return val
    def SetEdgeMode(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetEdgeMode,(self,) + _args, _kwargs)
        return val
    def GetEdgeColour(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetEdgeColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetEdgeColour(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetEdgeColour,(self,) + _args, _kwargs)
        return val
    def SetLexer(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetLexer,(self,) + _args, _kwargs)
        return val
    def GetLexer(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_GetLexer,(self,) + _args, _kwargs)
        return val
    def Colourise(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_Colourise,(self,) + _args, _kwargs)
        return val
    def SetProperty(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetProperty,(self,) + _args, _kwargs)
        return val
    def SetKeywords(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextCtrl_SetKeywords,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStyledTextCtrl instance at %s>" % (self.this,)
class wxStyledTextCtrl(wxStyledTextCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stcc.new_wxStyledTextCtrl,_args,_kwargs)
        self.thisown = 1




class wxStyledTextEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,stcc=stcc):
        if self.thisown == 1 :
            stcc.delete_wxStyledTextEvent(self)
    def SetPosition(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetPosition,(self,) + _args, _kwargs)
        return val
    def SetKey(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetKey,(self,) + _args, _kwargs)
        return val
    def SetModifiers(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetModifiers,(self,) + _args, _kwargs)
        return val
    def SetModificationType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetModificationType,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetText,(self,) + _args, _kwargs)
        return val
    def SetLength(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetLength,(self,) + _args, _kwargs)
        return val
    def SetLinesAdded(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetLinesAdded,(self,) + _args, _kwargs)
        return val
    def SetLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetLine,(self,) + _args, _kwargs)
        return val
    def SetFoldLevelNow(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetFoldLevelNow,(self,) + _args, _kwargs)
        return val
    def SetFoldLevelPrev(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetFoldLevelPrev,(self,) + _args, _kwargs)
        return val
    def SetMargin(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetMargin,(self,) + _args, _kwargs)
        return val
    def SetMessage(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetMessage,(self,) + _args, _kwargs)
        return val
    def SetWParam(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetWParam,(self,) + _args, _kwargs)
        return val
    def SetLParam(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_SetLParam,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetPosition,(self,) + _args, _kwargs)
        return val
    def GetKey(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetKey,(self,) + _args, _kwargs)
        return val
    def GetModifiers(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetModifiers,(self,) + _args, _kwargs)
        return val
    def GetModificationType(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetModificationType,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetText,(self,) + _args, _kwargs)
        return val
    def GetLength(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetLength,(self,) + _args, _kwargs)
        return val
    def GetLinesAdded(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetLinesAdded,(self,) + _args, _kwargs)
        return val
    def GetLine(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetLine,(self,) + _args, _kwargs)
        return val
    def GetFoldLevelNow(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetFoldLevelNow,(self,) + _args, _kwargs)
        return val
    def GetFoldLevelPrev(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetFoldLevelPrev,(self,) + _args, _kwargs)
        return val
    def GetMargin(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetMargin,(self,) + _args, _kwargs)
        return val
    def GetMessage(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetMessage,(self,) + _args, _kwargs)
        return val
    def GetWParam(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetWParam,(self,) + _args, _kwargs)
        return val
    def GetLParam(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetLParam,(self,) + _args, _kwargs)
        return val
    def GetShift(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetShift,(self,) + _args, _kwargs)
        return val
    def GetControl(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetControl,(self,) + _args, _kwargs)
        return val
    def GetAlt(self, *_args, **_kwargs):
        val = apply(stcc.wxStyledTextEvent_GetAlt,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStyledTextEvent instance at %s>" % (self.this,)
class wxStyledTextEvent(wxStyledTextEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stcc.new_wxStyledTextEvent,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxSTC_UndoCollectNone = stcc.wxSTC_UndoCollectNone
wxSTC_UndoCollectAutoStart = stcc.wxSTC_UndoCollectAutoStart
wxSTC_EOL_CRLF = stcc.wxSTC_EOL_CRLF
wxSTC_EOL_CR = stcc.wxSTC_EOL_CR
wxSTC_EOL_LF = stcc.wxSTC_EOL_LF
wxSTC_EDGE_NONE = stcc.wxSTC_EDGE_NONE
wxSTC_EDGE_LINE = stcc.wxSTC_EDGE_LINE
wxSTC_EDGE_BACKGROUND = stcc.wxSTC_EDGE_BACKGROUND
wxSTC_LEX_STYLE_MAX = stcc.wxSTC_LEX_STYLE_MAX
wxSTC_STYLE_DEFAULT = stcc.wxSTC_STYLE_DEFAULT
wxSTC_STYLE_LINENUMBER = stcc.wxSTC_STYLE_LINENUMBER
wxSTC_STYLE_BRACELIGHT = stcc.wxSTC_STYLE_BRACELIGHT
wxSTC_STYLE_BRACEBAD = stcc.wxSTC_STYLE_BRACEBAD
wxSTC_STYLE_CONTROLCHAR = stcc.wxSTC_STYLE_CONTROLCHAR
wxSTC_STYLE_MAX = stcc.wxSTC_STYLE_MAX
wxSTC_STYLE_MASK = stcc.wxSTC_STYLE_MASK
wxSTC_MARKER_MAX = stcc.wxSTC_MARKER_MAX
wxSTC_MARK_CIRCLE = stcc.wxSTC_MARK_CIRCLE
wxSTC_MARK_ROUNDRECT = stcc.wxSTC_MARK_ROUNDRECT
wxSTC_MARK_ARROW = stcc.wxSTC_MARK_ARROW
wxSTC_MARK_SMALLRECT = stcc.wxSTC_MARK_SMALLRECT
wxSTC_MARK_SHORTARROW = stcc.wxSTC_MARK_SHORTARROW
wxSTC_MARK_EMPTY = stcc.wxSTC_MARK_EMPTY
wxSTC_INDIC_PLAIN = stcc.wxSTC_INDIC_PLAIN
wxSTC_INDIC_SQUIGGLE = stcc.wxSTC_INDIC_SQUIGGLE
wxSTC_INDIC_TT = stcc.wxSTC_INDIC_TT
wxSTC_INDIC0_MASK = stcc.wxSTC_INDIC0_MASK
wxSTC_INDIC1_MASK = stcc.wxSTC_INDIC1_MASK
wxSTC_INDIC2_MASK = stcc.wxSTC_INDIC2_MASK
wxSTC_INDICS_MASK = stcc.wxSTC_INDICS_MASK
wxSTC_CMD_LINEDOWN = stcc.wxSTC_CMD_LINEDOWN
wxSTC_CMD_LINEDOWNEXTEND = stcc.wxSTC_CMD_LINEDOWNEXTEND
wxSTC_CMD_LINEUP = stcc.wxSTC_CMD_LINEUP
wxSTC_CMD_LINEUPEXTEND = stcc.wxSTC_CMD_LINEUPEXTEND
wxSTC_CMD_CHARLEFT = stcc.wxSTC_CMD_CHARLEFT
wxSTC_CMD_CHARLEFTEXTEND = stcc.wxSTC_CMD_CHARLEFTEXTEND
wxSTC_CMD_CHARRIGHT = stcc.wxSTC_CMD_CHARRIGHT
wxSTC_CMD_CHARRIGHTEXTEND = stcc.wxSTC_CMD_CHARRIGHTEXTEND
wxSTC_CMD_WORDLEFT = stcc.wxSTC_CMD_WORDLEFT
wxSTC_CMD_WORDLEFTEXTEND = stcc.wxSTC_CMD_WORDLEFTEXTEND
wxSTC_CMD_WORDRIGHT = stcc.wxSTC_CMD_WORDRIGHT
wxSTC_CMD_WORDRIGHTEXTEND = stcc.wxSTC_CMD_WORDRIGHTEXTEND
wxSTC_CMD_HOME = stcc.wxSTC_CMD_HOME
wxSTC_CMD_HOMEEXTEND = stcc.wxSTC_CMD_HOMEEXTEND
wxSTC_CMD_LINEEND = stcc.wxSTC_CMD_LINEEND
wxSTC_CMD_LINEENDEXTEND = stcc.wxSTC_CMD_LINEENDEXTEND
wxSTC_CMD_DOCUMENTSTART = stcc.wxSTC_CMD_DOCUMENTSTART
wxSTC_CMD_DOCUMENTSTARTEXTEND = stcc.wxSTC_CMD_DOCUMENTSTARTEXTEND
wxSTC_CMD_DOCUMENTEND = stcc.wxSTC_CMD_DOCUMENTEND
wxSTC_CMD_DOCUMENTENDEXTEND = stcc.wxSTC_CMD_DOCUMENTENDEXTEND
wxSTC_CMD_PAGEUP = stcc.wxSTC_CMD_PAGEUP
wxSTC_CMD_PAGEUPEXTEND = stcc.wxSTC_CMD_PAGEUPEXTEND
wxSTC_CMD_PAGEDOWN = stcc.wxSTC_CMD_PAGEDOWN
wxSTC_CMD_PAGEDOWNEXTEND = stcc.wxSTC_CMD_PAGEDOWNEXTEND
wxSTC_CMD_EDITTOGGLEOVERTYPE = stcc.wxSTC_CMD_EDITTOGGLEOVERTYPE
wxSTC_CMD_CANCEL = stcc.wxSTC_CMD_CANCEL
wxSTC_CMD_DELETEBACK = stcc.wxSTC_CMD_DELETEBACK
wxSTC_CMD_TAB = stcc.wxSTC_CMD_TAB
wxSTC_CMD_BACKTAB = stcc.wxSTC_CMD_BACKTAB
wxSTC_CMD_NEWLINE = stcc.wxSTC_CMD_NEWLINE
wxSTC_CMD_FORMFEED = stcc.wxSTC_CMD_FORMFEED
wxSTC_CMD_VCHOME = stcc.wxSTC_CMD_VCHOME
wxSTC_CMD_VCHOMEEXTEND = stcc.wxSTC_CMD_VCHOMEEXTEND
wxSTC_CMD_ZOOMIN = stcc.wxSTC_CMD_ZOOMIN
wxSTC_CMD_ZOOMOUT = stcc.wxSTC_CMD_ZOOMOUT
wxSTC_CMD_DELWORDLEFT = stcc.wxSTC_CMD_DELWORDLEFT
wxSTC_CMD_DELWORDRIGHT = stcc.wxSTC_CMD_DELWORDRIGHT
wxSTC_LEX_CONTAINER = stcc.wxSTC_LEX_CONTAINER
wxSTC_LEX_NULL = stcc.wxSTC_LEX_NULL
wxSTC_LEX_PYTHON = stcc.wxSTC_LEX_PYTHON
wxSTC_LEX_CPP = stcc.wxSTC_LEX_CPP
wxSTC_LEX_HTML = stcc.wxSTC_LEX_HTML
wxSTC_LEX_XML = stcc.wxSTC_LEX_XML
wxSTC_LEX_PERL = stcc.wxSTC_LEX_PERL
wxSTC_LEX_SQL = stcc.wxSTC_LEX_SQL
wxSTC_LEX_VB = stcc.wxSTC_LEX_VB
wxSTC_LEX_PROPERTIES = stcc.wxSTC_LEX_PROPERTIES
wxSTC_LEX_ERRORLIST = stcc.wxSTC_LEX_ERRORLIST
wxSTC_LEX_MAKEFILE = stcc.wxSTC_LEX_MAKEFILE
wxSTC_LEX_BATCH = stcc.wxSTC_LEX_BATCH
wxSTC_CARET_SLOP = stcc.wxSTC_CARET_SLOP
WXSTC_CARET_CENTER = stcc.WXSTC_CARET_CENTER
wxSTC_CARET_STRICT = stcc.wxSTC_CARET_STRICT
wxSTC_MARGIN_SYMBOL = stcc.wxSTC_MARGIN_SYMBOL
wxSTC_MARGIN_NUMBER = stcc.wxSTC_MARGIN_NUMBER
wxSTCNameStr = stcc.wxSTCNameStr
wxEVT_STC_CHANGE = stcc.wxEVT_STC_CHANGE
wxEVT_STC_STYLENEEDED = stcc.wxEVT_STC_STYLENEEDED
wxEVT_STC_CHARADDED = stcc.wxEVT_STC_CHARADDED
wxEVT_STC_UPDATEUI = stcc.wxEVT_STC_UPDATEUI
wxEVT_STC_SAVEPOINTREACHED = stcc.wxEVT_STC_SAVEPOINTREACHED
wxEVT_STC_SAVEPOINTLEFT = stcc.wxEVT_STC_SAVEPOINTLEFT
wxEVT_STC_ROMODIFYATTEMPT = stcc.wxEVT_STC_ROMODIFYATTEMPT
wxEVT_STC_DOUBLECLICK = stcc.wxEVT_STC_DOUBLECLICK
wxEVT_STC_MODIFIED = stcc.wxEVT_STC_MODIFIED
wxEVT_STC_KEY = stcc.wxEVT_STC_KEY
wxEVT_STC_MACRORECORD = stcc.wxEVT_STC_MACRORECORD
wxEVT_STC_MARGINCLICK = stcc.wxEVT_STC_MARGINCLICK
wxEVT_STC_NEEDSHOWN = stcc.wxEVT_STC_NEEDSHOWN
