////////////////////////////////////////////////////////////////////////////
// Name:        ScintillaWX.h
// Purpose:     A wxWindows implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatWX.cpp.  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxStyledTextCtrl to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ScintillaWX_h__
#define __ScintillaWX_h__

//----------------------------------------------------------------------

#include "Platform.h"

#include "Scintilla.h"
#ifdef SCI_LEXER
#include "SciLexer.h"
#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#endif
#include "ContractionState.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "AutoComplete.h"
#include "Document.h"
#include "Editor.h"
#include "ScintillaBase.h"

#include <wx/wx.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>

//----------------------------------------------------------------------

class wxStyledTextCtrl;           // forward
class ScintillaWX;


//----------------------------------------------------------------------
// Helper classes

class wxSTCDropTarget : public wxTextDropTarget {
public:
    void SetScintilla(ScintillaWX* swx) {
        this->swx = swx;
    }

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
    wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void OnLeave();

private:
    ScintillaWX* swx;
};


//----------------------------------------------------------------------

class ScintillaWX : public ScintillaBase {
public:

    ScintillaWX(wxStyledTextCtrl* win);
    ~ScintillaWX();

    // base class virtuals
    virtual void Initialise();
    virtual void Finalise();
    virtual void StartDrag();
    virtual void SetTicking(bool on);
    virtual void SetMouseCapture(bool on);
    virtual bool HaveMouseCapture();
    virtual void ScrollText(int linesToMove);
    virtual void SetVerticalScrollPos();
    virtual void SetHorizontalScrollPos();
    virtual bool ModifyScrollBars(int nMax, int nPage);
    virtual void Copy();
    virtual void Paste();
    virtual void CreateCallTipWindow(PRectangle rc);
    virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true);
    virtual void ClaimSelection();

    virtual LRESULT DefWndProc(UINT iMessage, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam);

    virtual void NotifyChange();
    virtual void NotifyParent(SCNotification scn);


    // Event delegates
    void DoPaint(wxDC* dc, wxRect rect);
    void DoHScroll(int type, int pos);
    void DoVScroll(int type, int pos);
    void DoSize(int width, int height);
    void DoLoseFocus();
    void DoGainFocus();
    void DoSysColourChange();
    void DoButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
    void DoButtonUp(Point pt, unsigned int curTime, bool ctrl);
    void DoButtonMove(Point pt);
    void DoAddChar(char ch);
    int  DoKeyDown(int key, bool shift, bool ctrl, bool alt);
    void DoTick() { Tick(); }

    bool DoDropText(long x, long y, const wxString& data);
    wxDragResult DoDragEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult DoDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void DoDragLeave();

    void DoCommand(int ID);
    void DoContextMenu(Point pt);


    // helpers
    void FullPaint();
    bool CanPaste();
    bool GetHideSelection() { return hideSelection; }
    void DoScrollToLine(int line);
    void DoScrollToColumn(int column);

private:
    bool                capturedMouse;
    wxStyledTextCtrl*   stc;

    wxTextDataObject    textDO;
    wxSTCDropTarget     dropTarget;
    wxDragResult        dragResult;
};

//----------------------------------------------------------------------
#endif
