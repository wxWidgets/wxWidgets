# This file was created automatically by SWIG.
import stc_c

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

def EVT_STC_MACRORECORD(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MACRORECORD, fn)

def EVT_STC_MARGINCLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MARGINCLICK, fn)

def EVT_STC_NEEDSHOWN(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_NEEDSHOWN, fn)



class wxStyledTextCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetText,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetText,(self,) + _args, _kwargs)
        return val
    def GetLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLine,(self,) + _args, _kwargs)
        return val
    def ReplaceSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ReplaceSelection,(self,) + _args, _kwargs)
        return val
    def SetReadOnly(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetReadOnly,(self,) + _args, _kwargs)
        return val
    def GetReadOnly(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetReadOnly,(self,) + _args, _kwargs)
        return val
    def GetTextRange(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetTextRange,(self,) + _args, _kwargs)
        return val
    def GetStyledTextRange(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyledTextRange,(self,) + _args, _kwargs)
        return val
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
    def GetCharAt(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCharAt,(self,) + _args, _kwargs)
        return val
    def GetStyleAt(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyleAt,(self,) + _args, _kwargs)
        return val
    def SetStyleBits(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyleBits,(self,) + _args, _kwargs)
        return val
    def GetStyleBits(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetStyleBits,(self,) + _args, _kwargs)
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
    def CanPaste(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanPaste,(self,) + _args, _kwargs)
        return val
    def ClearClipbrd(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ClearClipbrd,(self,) + _args, _kwargs)
        return val
    def Undo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Undo,(self,) + _args, _kwargs)
        return val
    def CanUndo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanUndo,(self,) + _args, _kwargs)
        return val
    def EmptyUndoBuffer(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EmptyUndoBuffer,(self,) + _args, _kwargs)
        return val
    def Redo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_Redo,(self,) + _args, _kwargs)
        return val
    def CanRedo(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CanRedo,(self,) + _args, _kwargs)
        return val
    def SetUndoCollection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetUndoCollection,(self,) + _args, _kwargs)
        return val
    def GetUndoCollection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetUndoCollection,(self,) + _args, _kwargs)
        return val
    def BeginUndoAction(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BeginUndoAction,(self,) + _args, _kwargs)
        return val
    def EndUndoAction(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EndUndoAction,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelection,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelection,(self,) + _args, _kwargs)
        return val
    def GetSelectedText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelectedText,(self,) + _args, _kwargs)
        return val
    def HideSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_HideSelection,(self,) + _args, _kwargs)
        return val
    def GetHideSelection(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetHideSelection,(self,) + _args, _kwargs)
        return val
    def GetTextLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetTextLength,(self,) + _args, _kwargs)
        return val
    def GetFirstVisibleLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetFirstVisibleLine,(self,) + _args, _kwargs)
        return val
    def GetModified(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetModified,(self,) + _args, _kwargs)
        return val
    def GetLineCount(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineCount,(self,) + _args, _kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetLineFromPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineFromPos,(self,) + _args, _kwargs)
        return val
    def GetLineStartPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineStartPos,(self,) + _args, _kwargs)
        return val
    def GetLineLengthAtPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineLengthAtPos,(self,) + _args, _kwargs)
        return val
    def GetLineLength(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLineLength,(self,) + _args, _kwargs)
        return val
    def GetCurrentLineText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurrentLineText,(self,) + _args, _kwargs)
        return val
    def GetCurrentLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurrentLine,(self,) + _args, _kwargs)
        return val
    def PositionFromPoint(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PositionFromPoint,(self,) + _args, _kwargs)
        return val
    def LineFromPoint(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_LineFromPoint,(self,) + _args, _kwargs)
        return val
    def PointFromPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PointFromPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetCurrentPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCurrentPos,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetAnchor,(self,) + _args, _kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SelectAll,(self,) + _args, _kwargs)
        return val
    def SetCurrentPosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCurrentPosition,(self,) + _args, _kwargs)
        return val
    def SetAnchor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetAnchor,(self,) + _args, _kwargs)
        return val
    def GotoPos(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GotoPos,(self,) + _args, _kwargs)
        return val
    def GotoLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GotoLine,(self,) + _args, _kwargs)
        return val
    def ChangePosition(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ChangePosition,(self,) + _args, _kwargs)
        return val
    def PageMove(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_PageMove,(self,) + _args, _kwargs)
        return val
    def ScrollBy(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ScrollBy,(self,) + _args, _kwargs)
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ScrollToLine,(self,) + _args, _kwargs)
        return val
    def ScrollToColumn(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ScrollToColumn,(self,) + _args, _kwargs)
        return val
    def EnsureCaretVisible(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_EnsureCaretVisible,(self,) + _args, _kwargs)
        return val
    def SetCaretPolicy(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretPolicy,(self,) + _args, _kwargs)
        return val
    def GetSelectionType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetSelectionType,(self,) + _args, _kwargs)
        return val
    def FindText(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_FindText,(self,) + _args, _kwargs)
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
    def GetViewWhitespace(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetViewWhitespace,(self,) + _args, _kwargs)
        return val
    def SetViewWhitespace(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetViewWhitespace,(self,) + _args, _kwargs)
        return val
    def GetEOLMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEOLMode,(self,) + _args, _kwargs)
        return val
    def SetEOLMode(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetEOLMode,(self,) + _args, _kwargs)
        return val
    def GetViewEOL(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetViewEOL,(self,) + _args, _kwargs)
        return val
    def SetViewEOL(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetViewEOL,(self,) + _args, _kwargs)
        return val
    def ConvertEOL(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_ConvertEOL,(self,) + _args, _kwargs)
        return val
    def GetEndStyled(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetEndStyled,(self,) + _args, _kwargs)
        return val
    def StartStyling(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StartStyling,(self,) + _args, _kwargs)
        return val
    def SetStyleFor(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyleFor,(self,) + _args, _kwargs)
        return val
    def SetStyleBytes(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetStyleBytes,(self,) + _args, _kwargs)
        return val
    def StyleClearAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleClearAll,(self,) + _args, _kwargs)
        return val
    def StyleResetDefault(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleResetDefault,(self,) + _args, _kwargs)
        return val
    def StyleSetSpec(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetSpec,(self,) + _args, _kwargs)
        return val
    def StyleSetForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetForeground,(self,) + _args, _kwargs)
        return val
    def StyleSetBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetBackground,(self,) + _args, _kwargs)
        return val
    def StyleSetFont(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFont,(self,) + _args, _kwargs)
        return val
    def StyleSetFontAttr(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFontAttr,(self,) + _args, _kwargs)
        return val
    def StyleSetBold(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetBold,(self,) + _args, _kwargs)
        return val
    def StyleSetItalic(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetItalic,(self,) + _args, _kwargs)
        return val
    def StyleSetFaceName(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetFaceName,(self,) + _args, _kwargs)
        return val
    def StyleSetSize(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetSize,(self,) + _args, _kwargs)
        return val
    def StyleSetEOLFilled(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_StyleSetEOLFilled,(self,) + _args, _kwargs)
        return val
    def GetLeftMargin(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetLeftMargin,(self,) + _args, _kwargs)
        return val
    def GetRightMargin(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetRightMargin,(self,) + _args, _kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetMargins,(self,) + _args, _kwargs)
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
    def SetSelectionForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelectionForeground,(self,) + _args, _kwargs)
        return val
    def SetSelectionBackground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetSelectionBackground,(self,) + _args, _kwargs)
        return val
    def SetCaretForeground(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretForeground,(self,) + _args, _kwargs)
        return val
    def GetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetCaretPeriod(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetCaretPeriod,(self,) + _args, _kwargs)
        return val
    def SetBufferedDraw(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetBufferedDraw,(self,) + _args, _kwargs)
        return val
    def SetTabWidth(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetTabWidth,(self,) + _args, _kwargs)
        return val
    def SetWordChars(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetWordChars,(self,) + _args, _kwargs)
        return val
    def BraceHighlight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceHighlight,(self,) + _args, _kwargs)
        return val
    def BraceBadlight(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceBadlight,(self,) + _args, _kwargs)
        return val
    def BraceMatch(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_BraceMatch,(self,) + _args, _kwargs)
        return val
    def MarkerDefine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDefine,(self,) + _args, _kwargs)
        return val
    def MarkerSetType(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerSetType,(self,) + _args, _kwargs)
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
    def MarkerGetNextLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerGetNextLine,(self,) + _args, _kwargs)
        return val
    def MarkerGetPrevLine(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerGetPrevLine,(self,) + _args, _kwargs)
        return val
    def MarkerLineFromHandle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerLineFromHandle,(self,) + _args, _kwargs)
        return val
    def MarkerDeleteHandle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_MarkerDeleteHandle,(self,) + _args, _kwargs)
        return val
    def IndicatorSetStyle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorSetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorGetStyle(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorGetStyle,(self,) + _args, _kwargs)
        return val
    def IndicatorSetColour(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_IndicatorSetColour,(self,) + _args, _kwargs)
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
    def AutoCompPosAtStart(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompPosAtStart,(self,) + _args, _kwargs)
        return val
    def AutoCompComplete(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompComplete,(self,) + _args, _kwargs)
        return val
    def AutoCompStopChars(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_AutoCompStopChars,(self,) + _args, _kwargs)
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
    def CmdKeyAssign(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyAssign,(self,) + _args, _kwargs)
        return val
    def CmdKeyClear(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyClear,(self,) + _args, _kwargs)
        return val
    def CmdKeyClearAll(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyClearAll,(self,) + _args, _kwargs)
        return val
    def CmdKeyExecute(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_CmdKeyExecute,(self,) + _args, _kwargs)
        return val
    def FormatRange(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_FormatRange,(self,) + _args, _kwargs)
        return val
    def GetDocument(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_GetDocument,(self,) + _args, _kwargs)
        return val
    def SetDocument(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetDocument,(self,) + _args, _kwargs)
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
    def SetKeywords(self, *_args, **_kwargs):
        val = apply(stc_c.wxStyledTextCtrl_SetKeywords,(self,) + _args, _kwargs)
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

wxSTC_UndoCollectNone = stc_c.wxSTC_UndoCollectNone
wxSTC_UndoCollectAutoStart = stc_c.wxSTC_UndoCollectAutoStart
wxSTC_EOL_CRLF = stc_c.wxSTC_EOL_CRLF
wxSTC_EOL_CR = stc_c.wxSTC_EOL_CR
wxSTC_EOL_LF = stc_c.wxSTC_EOL_LF
wxSTC_EDGE_NONE = stc_c.wxSTC_EDGE_NONE
wxSTC_EDGE_LINE = stc_c.wxSTC_EDGE_LINE
wxSTC_EDGE_BACKGROUND = stc_c.wxSTC_EDGE_BACKGROUND
wxSTC_LEX_STYLE_MAX = stc_c.wxSTC_LEX_STYLE_MAX
wxSTC_STYLE_DEFAULT = stc_c.wxSTC_STYLE_DEFAULT
wxSTC_STYLE_LINENUMBER = stc_c.wxSTC_STYLE_LINENUMBER
wxSTC_STYLE_BRACELIGHT = stc_c.wxSTC_STYLE_BRACELIGHT
wxSTC_STYLE_BRACEBAD = stc_c.wxSTC_STYLE_BRACEBAD
wxSTC_STYLE_CONTROLCHAR = stc_c.wxSTC_STYLE_CONTROLCHAR
wxSTC_STYLE_MAX = stc_c.wxSTC_STYLE_MAX
wxSTC_STYLE_MASK = stc_c.wxSTC_STYLE_MASK
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
wxSTC_MASK_FOLDERS = stc_c.wxSTC_MASK_FOLDERS
wxSTC_INDIC_MAX = stc_c.wxSTC_INDIC_MAX
wxSTC_INDIC_PLAIN = stc_c.wxSTC_INDIC_PLAIN
wxSTC_INDIC_SQUIGGLE = stc_c.wxSTC_INDIC_SQUIGGLE
wxSTC_INDIC_TT = stc_c.wxSTC_INDIC_TT
wxSTC_INDIC0_MASK = stc_c.wxSTC_INDIC0_MASK
wxSTC_INDIC1_MASK = stc_c.wxSTC_INDIC1_MASK
wxSTC_INDIC2_MASK = stc_c.wxSTC_INDIC2_MASK
wxSTC_INDICS_MASK = stc_c.wxSTC_INDICS_MASK
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
wxSTC_CARET_SLOP = stc_c.wxSTC_CARET_SLOP
WXSTC_CARET_CENTER = stc_c.WXSTC_CARET_CENTER
wxSTC_CARET_STRICT = stc_c.wxSTC_CARET_STRICT
wxSTC_MARGIN_SYMBOL = stc_c.wxSTC_MARGIN_SYMBOL
wxSTC_MARGIN_NUMBER = stc_c.wxSTC_MARGIN_NUMBER
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


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxStyledTextEventPtr       = wxStyledTextEventPtr
