////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/ScintillaWX.h
// Purpose:     A wxWidgets implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatWX.cpp.  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxStyledTextCtrl to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ScintillaWX_h__
#define __ScintillaWX_h__
#include "wx/defs.h"

//----------------------------------------------------------------------

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

// These are all Scintilla headers
#include "Platform.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "Scintilla.h"
#include "ScintillaWidget.h"
#ifdef SCI_LEXER
#include "SciLexer.h"
#include "PropSetSimple.h"
#include "ILexer.h"
#include "LexerModule.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "WordList.h"
#endif
#include "ContractionState.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "AutoComplete.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"
#include "MarginView.h"
#include "EditView.h"
#include "Editor.h"
#include "PropSetSimple.h"
#include "ScintillaBase.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h"                     // HBITMAP
#endif
#if wxUSE_DRAG_AND_DROP
#include "wx/timer.h"
#endif

// Define this if there is a standard clipboard format for rectangular
// text selection under the current platform.
#if defined(__WXMSW__) || defined(__WXGTK__)
    #define wxHAVE_STC_RECT_FORMAT
#endif

//----------------------------------------------------------------------


class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_STC wxStyledTextCtrl;           // forward
class ScintillaWX;
class wxSTCTimer;
class SurfaceData;

//----------------------------------------------------------------------
// Helper classes

#if wxUSE_DRAG_AND_DROP
class wxSTCDropTarget : public wxTextDropTarget {
public:
    void SetScintilla(ScintillaWX* swx) {
        m_swx = swx;
    }

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data) wxOVERRIDE;
    wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE;
    wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE;
    void OnLeave() wxOVERRIDE;

private:
    ScintillaWX* m_swx;
};
#endif

//----------------------------------------------------------------------

class ScintillaWX : public ScintillaBase {
public:

    ScintillaWX(wxStyledTextCtrl* win);
    ~ScintillaWX();

    // base class virtuals
    virtual void Initialise() wxOVERRIDE;
    virtual void Finalise() wxOVERRIDE;
    virtual void StartDrag() wxOVERRIDE;
    virtual bool SetIdle(bool on) wxOVERRIDE;
    virtual void SetMouseCapture(bool on) wxOVERRIDE;
    virtual bool HaveMouseCapture() wxOVERRIDE;
    virtual void ScrollText(int linesToMove) wxOVERRIDE;
    virtual void SetVerticalScrollPos() wxOVERRIDE;
    virtual void SetHorizontalScrollPos() wxOVERRIDE;
    virtual bool ModifyScrollBars(int nMax, int nPage) wxOVERRIDE;
    virtual void Copy() wxOVERRIDE;
    virtual void Paste() wxOVERRIDE;
    virtual void CopyToClipboard(const SelectionText &selectedText) wxOVERRIDE;

    virtual void CreateCallTipWindow(PRectangle rc) wxOVERRIDE;
    virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true) wxOVERRIDE;
    virtual void ClaimSelection() wxOVERRIDE;

    virtual sptr_t DefWndProc(unsigned int iMessage,
                              uptr_t wParam,
                              sptr_t lParam) wxOVERRIDE;
    virtual sptr_t WndProc(unsigned int iMessage,
                           uptr_t wParam,
                           sptr_t lParam) wxOVERRIDE;

    virtual void NotifyChange() wxOVERRIDE;
    virtual void NotifyParent(SCNotification scn) wxOVERRIDE;

    virtual void CancelModes() wxOVERRIDE;

    virtual void UpdateSystemCaret() wxOVERRIDE;
    virtual bool FineTickerAvailable() wxOVERRIDE;
    virtual bool FineTickerRunning(TickReason reason) wxOVERRIDE;
    virtual void FineTickerStart(TickReason reason, int millis, int tolerance) wxOVERRIDE;
    virtual void FineTickerCancel(TickReason reason) wxOVERRIDE;

    // Event delegates
    void DoPaint(wxDC* dc, wxRect rect);
    void DoHScroll(int type, int pos);
    void DoVScroll(int type, int pos);
    void DoSize(int width, int height);
    void DoLoseFocus();
    void DoGainFocus();
    void DoInvalidateStyleData();
    void DoLeftButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
    void DoRightButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
    void DoLeftButtonUp(Point pt, unsigned int curTime, bool ctrl);
    void DoLeftButtonMove(Point pt);
    void DoMiddleButtonUp(Point pt);
    void DoMouseWheel(wxMouseWheelAxis axis, int rotation, int delta,
                      int linesPerAction, int columnsPerAction,
                      bool ctrlDown, bool isPageScroll);
    void DoAddChar(int key);
    int  DoKeyDown(const wxKeyEvent& event, bool* consumed);
    void DoOnIdle(wxIdleEvent& evt);

#if wxUSE_DRAG_AND_DROP
    bool DoDropText(long x, long y, const wxString& data);
    wxDragResult DoDragEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult DoDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void DoDragLeave();
#endif

    void DoCommand(int ID);
    bool DoContextMenu(Point pt);
    void DoOnListBox();
    void DoMouseCaptureLost();


    // helpers
    void FullPaint();
    void FullPaintDC(wxDC* dc);
    bool CanPaste() wxOVERRIDE;
    bool GetHideSelection() { return view.hideSelection; }
    void DoScrollToLine(int line);
    void DoScrollToColumn(int column);
    void ClipChildren(wxDC& dc, PRectangle rect);
    void SetUseAntiAliasing(bool useAA);
    bool GetUseAntiAliasing();
    SurfaceData* GetSurfaceData() const {return m_surfaceData;}
    void SetPaintAbandoned(){paintState = paintAbandoned;}
    void DoMarkerDefineBitmap(int markerNumber, const wxBitmap& bmp);
    void DoRegisterImage(int type, const wxBitmap& bmp);

private:
    bool                capturedMouse;
    bool                focusEvent;
    wxStyledTextCtrl*   stc;

    WX_DECLARE_HASH_MAP(TickReason, wxSTCTimer*, wxIntegerHash, wxIntegerEqual, TimersHash);
    TimersHash          timers;

#if wxUSE_DRAG_AND_DROP
    wxSTCDropTarget*    dropTarget;
    wxDragResult        dragResult;
#endif

    int                 wheelVRotation;
    int                 wheelHRotation;
    SurfaceData*        m_surfaceData;

    // For use in creating a system caret
    bool HasCaretSizeChanged();
    bool CreateSystemCaret();
    bool DestroySystemCaret();

    static sptr_t DirectFunction(ScintillaWX* swx, unsigned int iMessage,
                                 uptr_t wParam, sptr_t lParam);
#ifdef __WXMSW__
    HBITMAP sysCaretBitmap;
    int sysCaretWidth;
    int sysCaretHeight;
#endif

#ifdef wxHAVE_STC_RECT_FORMAT
    // The presence of this format on the clipboard indicates that the text is
    // a rectangular (and not the default linear) selection.
    wxDataFormat m_clipRectTextFormat;
#endif

#ifdef __WXMSW__
    // ScintillaWin
    HWND MainHWND() const wxNOEXCEPT;

    // DBCS
    void ImeStartComposition();
    void ImeEndComposition();
#endif

    friend class wxSTCCallTip;
    friend class wxSTCTimer; // To get access to TickReason declaration
};

//----------------------------------------------------------------------
#endif
