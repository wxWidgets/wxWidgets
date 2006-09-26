/////////////////////////////////////////////////////////////////////////////
// Name:        stc.i
// Purpose:     Wrappers for the wxStyledTextCtrl.
//
// Author:      Robin Dunn
//
// Created:     12-Oct-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The `StyledTextCtrl` provides a text editor that can used as a syntax
highlighting source code editor, or similar.  Lexers for several programming
languages are built-in."
%enddef

%module(package="wx", docstring=DOCSTRING) stc


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include <wx/stc/stc.h>

%}

//---------------------------------------------------------------------------

%import core.i
%import misc.i  // for DnD
//%import gdi.i   // for wxFontEncoding

%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING(STCNameStr);


%include _stc_docstrings.i

enum wxFontEncoding;  // forward declare

MustHaveApp(wxStyledTextCtrl);

//---------------------------------------------------------------------------
// Get all our defs from the REAL header file.

%include stc.h

%extend wxStyledTextCtrl {
    %pythoncode {
        GetCaretLineBack = GetCaretLineBackground
        SetCaretLineBack = SetCaretLineBackground
    }

    
    %property(Anchor, GetAnchor, SetAnchor, doc="See `GetAnchor` and `SetAnchor`");
    %property(BackSpaceUnIndents, GetBackSpaceUnIndents, SetBackSpaceUnIndents, doc="See `GetBackSpaceUnIndents` and `SetBackSpaceUnIndents`");
    %property(BufferedDraw, GetBufferedDraw, SetBufferedDraw, doc="See `GetBufferedDraw` and `SetBufferedDraw`");
    %property(CaretForeground, GetCaretForeground, SetCaretForeground, doc="See `GetCaretForeground` and `SetCaretForeground`");
    %property(CaretLineBack, GetCaretLineBack, SetCaretLineBack, doc="See `GetCaretLineBack` and `SetCaretLineBack`");
    %property(CaretLineBackAlpha, GetCaretLineBackAlpha, SetCaretLineBackAlpha, doc="See `GetCaretLineBackAlpha` and `SetCaretLineBackAlpha`");
    %property(CaretLineBackground, GetCaretLineBackground, SetCaretLineBackground, doc="See `GetCaretLineBackground` and `SetCaretLineBackground`");
    %property(CaretLineVisible, GetCaretLineVisible, SetCaretLineVisible, doc="See `GetCaretLineVisible` and `SetCaretLineVisible`");
    %property(CaretPeriod, GetCaretPeriod, SetCaretPeriod, doc="See `GetCaretPeriod` and `SetCaretPeriod`");
    %property(CaretSticky, GetCaretSticky, SetCaretSticky, doc="See `GetCaretSticky` and `SetCaretSticky`");
    %property(CaretWidth, GetCaretWidth, SetCaretWidth, doc="See `GetCaretWidth` and `SetCaretWidth`");
    %property(CodePage, GetCodePage, SetCodePage, doc="See `GetCodePage` and `SetCodePage`");
    %property(ControlCharSymbol, GetControlCharSymbol, SetControlCharSymbol, doc="See `GetControlCharSymbol` and `SetControlCharSymbol`");
    %property(CurLine, GetCurLine, doc="See `GetCurLine`");
    %property(CurLineRaw, GetCurLineRaw, doc="See `GetCurLineRaw`");
    %property(CurLineUTF8, GetCurLineUTF8, doc="See `GetCurLineUTF8`");
    %property(CurrentLine, GetCurrentLine, doc="See `GetCurrentLine`");
    %property(CurrentPos, GetCurrentPos, SetCurrentPos, doc="See `GetCurrentPos` and `SetCurrentPos`");
    %property(DocPointer, GetDocPointer, SetDocPointer, doc="See `GetDocPointer` and `SetDocPointer`");
    %property(EOLMode, GetEOLMode, SetEOLMode, doc="See `GetEOLMode` and `SetEOLMode`");
    %property(EdgeColour, GetEdgeColour, SetEdgeColour, doc="See `GetEdgeColour` and `SetEdgeColour`");
    %property(EdgeColumn, GetEdgeColumn, SetEdgeColumn, doc="See `GetEdgeColumn` and `SetEdgeColumn`");
    %property(EdgeMode, GetEdgeMode, SetEdgeMode, doc="See `GetEdgeMode` and `SetEdgeMode`");
    %property(EndAtLastLine, GetEndAtLastLine, SetEndAtLastLine, doc="See `GetEndAtLastLine` and `SetEndAtLastLine`");
    %property(EndStyled, GetEndStyled, doc="See `GetEndStyled`");
    %property(FirstVisibleLine, GetFirstVisibleLine, doc="See `GetFirstVisibleLine`");
    %property(HighlightGuide, GetHighlightGuide, SetHighlightGuide, doc="See `GetHighlightGuide` and `SetHighlightGuide`");
    %property(Indent, GetIndent, SetIndent, doc="See `GetIndent` and `SetIndent`");
    %property(IndentationGuides, GetIndentationGuides, SetIndentationGuides, doc="See `GetIndentationGuides` and `SetIndentationGuides`");
    %property(LastKeydownProcessed, GetLastKeydownProcessed, SetLastKeydownProcessed, doc="See `GetLastKeydownProcessed` and `SetLastKeydownProcessed`");
    %property(LayoutCache, GetLayoutCache, SetLayoutCache, doc="See `GetLayoutCache` and `SetLayoutCache`");
    %property(Length, GetLength, doc="See `GetLength`");
    %property(Lexer, GetLexer, SetLexer, doc="See `GetLexer` and `SetLexer`");
    %property(LineCount, GetLineCount, doc="See `GetLineCount`");
    %property(MarginLeft, GetMarginLeft, SetMarginLeft, doc="See `GetMarginLeft` and `SetMarginLeft`");
    %property(MarginRight, GetMarginRight, SetMarginRight, doc="See `GetMarginRight` and `SetMarginRight`");
    %property(MaxLineState, GetMaxLineState, doc="See `GetMaxLineState`");
    %property(ModEventMask, GetModEventMask, SetModEventMask, doc="See `GetModEventMask` and `SetModEventMask`");
    %property(Modify, GetModify, doc="See `GetModify`");
    %property(MouseDownCaptures, GetMouseDownCaptures, SetMouseDownCaptures, doc="See `GetMouseDownCaptures` and `SetMouseDownCaptures`");
    %property(MouseDwellTime, GetMouseDwellTime, SetMouseDwellTime, doc="See `GetMouseDwellTime` and `SetMouseDwellTime`");
    %property(Overtype, GetOvertype, SetOvertype, doc="See `GetOvertype` and `SetOvertype`");
    %property(PasteConvertEndings, GetPasteConvertEndings, SetPasteConvertEndings, doc="See `GetPasteConvertEndings` and `SetPasteConvertEndings`");
    %property(PrintColourMode, GetPrintColourMode, SetPrintColourMode, doc="See `GetPrintColourMode` and `SetPrintColourMode`");
    %property(PrintMagnification, GetPrintMagnification, SetPrintMagnification, doc="See `GetPrintMagnification` and `SetPrintMagnification`");
    %property(PrintWrapMode, GetPrintWrapMode, SetPrintWrapMode, doc="See `GetPrintWrapMode` and `SetPrintWrapMode`");
    %property(ReadOnly, GetReadOnly, SetReadOnly, doc="See `GetReadOnly` and `SetReadOnly`");
    %property(STCCursor, GetSTCCursor, SetSTCCursor, doc="See `GetSTCCursor` and `SetSTCCursor`");
    %property(STCFocus, GetSTCFocus, SetSTCFocus, doc="See `GetSTCFocus` and `SetSTCFocus`");
    %property(ScrollWidth, GetScrollWidth, SetScrollWidth, doc="See `GetScrollWidth` and `SetScrollWidth`");
    %property(SearchFlags, GetSearchFlags, SetSearchFlags, doc="See `GetSearchFlags` and `SetSearchFlags`");
    %property(SelAlpha, GetSelAlpha, SetSelAlpha, doc="See `GetSelAlpha` and `SetSelAlpha`");
    %property(SelectedText, GetSelectedText, doc="See `GetSelectedText`");
    %property(SelectedTextRaw, GetSelectedTextRaw, doc="See `GetSelectedTextRaw`");
    %property(SelectedTextUTF8, GetSelectedTextUTF8, doc="See `GetSelectedTextUTF8`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(SelectionEnd, GetSelectionEnd, SetSelectionEnd, doc="See `GetSelectionEnd` and `SetSelectionEnd`");
    %property(SelectionMode, GetSelectionMode, SetSelectionMode, doc="See `GetSelectionMode` and `SetSelectionMode`");
    %property(SelectionStart, GetSelectionStart, SetSelectionStart, doc="See `GetSelectionStart` and `SetSelectionStart`");
    %property(Status, GetStatus, SetStatus, doc="See `GetStatus` and `SetStatus`");
    %property(StyleBits, GetStyleBits, SetStyleBits, doc="See `GetStyleBits` and `SetStyleBits`");
    %property(StyleBitsNeeded, GetStyleBitsNeeded, doc="See `GetStyleBitsNeeded`");
    %property(TabIndents, GetTabIndents, SetTabIndents, doc="See `GetTabIndents` and `SetTabIndents`");
    %property(TabWidth, GetTabWidth, SetTabWidth, doc="See `GetTabWidth` and `SetTabWidth`");
    %property(TargetEnd, GetTargetEnd, SetTargetEnd, doc="See `GetTargetEnd` and `SetTargetEnd`");
    %property(TargetStart, GetTargetStart, SetTargetStart, doc="See `GetTargetStart` and `SetTargetStart`");
    %property(Text, GetText, SetText, doc="See `GetText` and `SetText`");
    %property(TextLength, GetTextLength, doc="See `GetTextLength`");
    %property(TextRaw, GetTextRaw, SetTextRaw, doc="See `GetTextRaw` and `SetTextRaw`");
    %property(TextUTF8, GetTextUTF8, SetTextUTF8, doc="See `GetTextUTF8` and `SetTextUTF8`");
    %property(TwoPhaseDraw, GetTwoPhaseDraw, SetTwoPhaseDraw, doc="See `GetTwoPhaseDraw` and `SetTwoPhaseDraw`");
    %property(UndoCollection, GetUndoCollection, SetUndoCollection, doc="See `GetUndoCollection` and `SetUndoCollection`");
    %property(UseAntiAliasing, GetUseAntiAliasing, SetUseAntiAliasing, doc="See `GetUseAntiAliasing` and `SetUseAntiAliasing`");
    %property(UseHorizontalScrollBar, GetUseHorizontalScrollBar, SetUseHorizontalScrollBar, doc="See `GetUseHorizontalScrollBar` and `SetUseHorizontalScrollBar`");
    %property(UseTabs, GetUseTabs, SetUseTabs, doc="See `GetUseTabs` and `SetUseTabs`");
    %property(UseVerticalScrollBar, GetUseVerticalScrollBar, SetUseVerticalScrollBar, doc="See `GetUseVerticalScrollBar` and `SetUseVerticalScrollBar`");
    %property(ViewEOL, GetViewEOL, SetViewEOL, doc="See `GetViewEOL` and `SetViewEOL`");
    %property(ViewWhiteSpace, GetViewWhiteSpace, SetViewWhiteSpace, doc="See `GetViewWhiteSpace` and `SetViewWhiteSpace`");
    %property(WrapMode, GetWrapMode, SetWrapMode, doc="See `GetWrapMode` and `SetWrapMode`");
    %property(WrapStartIndent, GetWrapStartIndent, SetWrapStartIndent, doc="See `GetWrapStartIndent` and `SetWrapStartIndent`");
    %property(WrapVisualFlags, GetWrapVisualFlags, SetWrapVisualFlags, doc="See `GetWrapVisualFlags` and `SetWrapVisualFlags`");
    %property(WrapVisualFlagsLocation, GetWrapVisualFlagsLocation, SetWrapVisualFlagsLocation, doc="See `GetWrapVisualFlagsLocation` and `SetWrapVisualFlagsLocation`");
    %property(XOffset, GetXOffset, SetXOffset, doc="See `GetXOffset` and `SetXOffset`");
    %property(Zoom, GetZoom, SetZoom, doc="See `GetZoom` and `SetZoom`");
}


%extend wxStyledTextEvent {
    %property(Alt, GetAlt, doc="See `GetAlt`");
    %property(Control, GetControl, doc="See `GetControl`");
    %property(DragAllowMove, GetDragAllowMove, SetDragAllowMove, doc="See `GetDragAllowMove` and `SetDragAllowMove`");
    %property(DragResult, GetDragResult, SetDragResult, doc="See `GetDragResult` and `SetDragResult`");
    %property(DragText, GetDragText, SetDragText, doc="See `GetDragText` and `SetDragText`");
    %property(FoldLevelNow, GetFoldLevelNow, SetFoldLevelNow, doc="See `GetFoldLevelNow` and `SetFoldLevelNow`");
    %property(FoldLevelPrev, GetFoldLevelPrev, SetFoldLevelPrev, doc="See `GetFoldLevelPrev` and `SetFoldLevelPrev`");
    %property(Key, GetKey, SetKey, doc="See `GetKey` and `SetKey`");
    %property(LParam, GetLParam, SetLParam, doc="See `GetLParam` and `SetLParam`");
    %property(Length, GetLength, SetLength, doc="See `GetLength` and `SetLength`");
    %property(Line, GetLine, SetLine, doc="See `GetLine` and `SetLine`");
    %property(LinesAdded, GetLinesAdded, SetLinesAdded, doc="See `GetLinesAdded` and `SetLinesAdded`");
    %property(ListType, GetListType, SetListType, doc="See `GetListType` and `SetListType`");
    %property(Margin, GetMargin, SetMargin, doc="See `GetMargin` and `SetMargin`");
    %property(Message, GetMessage, SetMessage, doc="See `GetMessage` and `SetMessage`");
    %property(ModificationType, GetModificationType, SetModificationType, doc="See `GetModificationType` and `SetModificationType`");
    %property(Modifiers, GetModifiers, SetModifiers, doc="See `GetModifiers` and `SetModifiers`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
    %property(Shift, GetShift, doc="See `GetShift`");
    %property(Text, GetText, SetText, doc="See `GetText` and `SetText`");
    %property(WParam, GetWParam, SetWParam, doc="See `GetWParam` and `SetWParam`");
    %property(X, GetX, SetX, doc="See `GetX` and `SetX`");
    %property(Y, GetY, SetY, doc="See `GetY` and `SetY`");
}

//---------------------------------------------------------------------------
// Python functions to act like the event macros

%pythoncode {
EVT_STC_CHANGE = wx.PyEventBinder( wxEVT_STC_CHANGE, 1 )
EVT_STC_STYLENEEDED = wx.PyEventBinder( wxEVT_STC_STYLENEEDED, 1 )
EVT_STC_CHARADDED = wx.PyEventBinder( wxEVT_STC_CHARADDED, 1 )
EVT_STC_SAVEPOINTREACHED = wx.PyEventBinder( wxEVT_STC_SAVEPOINTREACHED, 1 )
EVT_STC_SAVEPOINTLEFT = wx.PyEventBinder( wxEVT_STC_SAVEPOINTLEFT, 1 )
EVT_STC_ROMODIFYATTEMPT = wx.PyEventBinder( wxEVT_STC_ROMODIFYATTEMPT, 1 )
EVT_STC_KEY = wx.PyEventBinder( wxEVT_STC_KEY, 1 )
EVT_STC_DOUBLECLICK = wx.PyEventBinder( wxEVT_STC_DOUBLECLICK, 1 )
EVT_STC_UPDATEUI = wx.PyEventBinder( wxEVT_STC_UPDATEUI, 1 )
EVT_STC_MODIFIED = wx.PyEventBinder( wxEVT_STC_MODIFIED, 1 )
EVT_STC_MACRORECORD = wx.PyEventBinder( wxEVT_STC_MACRORECORD, 1 )
EVT_STC_MARGINCLICK = wx.PyEventBinder( wxEVT_STC_MARGINCLICK, 1 )
EVT_STC_NEEDSHOWN = wx.PyEventBinder( wxEVT_STC_NEEDSHOWN, 1 )
EVT_STC_PAINTED = wx.PyEventBinder( wxEVT_STC_PAINTED, 1 )
EVT_STC_USERLISTSELECTION = wx.PyEventBinder( wxEVT_STC_USERLISTSELECTION, 1 )
EVT_STC_URIDROPPED = wx.PyEventBinder( wxEVT_STC_URIDROPPED, 1 )
EVT_STC_DWELLSTART = wx.PyEventBinder( wxEVT_STC_DWELLSTART, 1 )
EVT_STC_DWELLEND = wx.PyEventBinder( wxEVT_STC_DWELLEND, 1 )
EVT_STC_START_DRAG = wx.PyEventBinder( wxEVT_STC_START_DRAG, 1 )
EVT_STC_DRAG_OVER = wx.PyEventBinder( wxEVT_STC_DRAG_OVER, 1 )
EVT_STC_DO_DROP = wx.PyEventBinder( wxEVT_STC_DO_DROP, 1 )
EVT_STC_ZOOM = wx.PyEventBinder( wxEVT_STC_ZOOM, 1 )
EVT_STC_HOTSPOT_CLICK = wx.PyEventBinder( wxEVT_STC_HOTSPOT_CLICK, 1 )
EVT_STC_HOTSPOT_DCLICK = wx.PyEventBinder( wxEVT_STC_HOTSPOT_DCLICK, 1 )
EVT_STC_CALLTIP_CLICK = wx.PyEventBinder( wxEVT_STC_CALLTIP_CLICK, 1 )
EVT_STC_AUTOCOMP_SELECTION = wx.PyEventBinder( wxEVT_STC_AUTOCOMP_SELECTION, 1 )
}

//---------------------------------------------------------------------------

%init %{
%}


//---------------------------------------------------------------------------

