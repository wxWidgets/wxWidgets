///////////////////////////////////////////////////////////////////////////////
// Name:        generic/caret.h
// Purpose:     generic wxCaret class
// Author:      Vadim Zeitlin (original code by Robert Roebling)
// Modified by:
// Created:     25.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CARET_H_
#define _WX_CARET_H_

#ifdef __GNUG__
#pragma interface "caret.h"
#endif

#include "wx/timer.h"

class wxCaret;

class wxCaretTimer : public wxTimer
{
public:
    wxCaretTimer(wxCaret *caret);
    virtual void Notify();

private:
    wxCaret *m_caret;
};

class wxCaret : public wxCaretBase
{
public:
    // ctors
    // -----
        // default - use Create()
    wxCaret() : m_timer(this) { InitGeneric(); }
        // creates a block caret associated with the given window
    wxCaret(wxWindowBase *window, int width, int height)
        : wxCaretBase(window, width, height), m_timer(this) { InitGeneric(); }
    wxCaret(wxWindowBase *window, const wxSize& size)
        : wxCaretBase(window, size), m_timer(this) { InitGeneric(); }

    virtual ~wxCaret();

    // implementation
    // --------------

    // called by wxWindow (not using the event tables)
    virtual void OnSetFocus();
    virtual void OnKillFocus();

    // called by wxCaretTimer
    void OnTimer();

protected:
    virtual void DoShow();
    virtual void DoHide();
    virtual void DoMove();

    // blink the caret once
    void Blink();

    // refresh the caret
    void Refresh();

    // draw the caret on the given DC
    void DoDraw(wxDC *dc);
    
private:
    // GTK specific initialization
    void InitGeneric();

    wxCaretTimer  m_timer;
    bool          m_blinkedOut,     // TRUE => caret hidden right now
                  m_hasFocus;       // TRUE => our window has focus
};

#endif // _WX_CARET_H_
