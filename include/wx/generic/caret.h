///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/caret.h
// Purpose:     generic wxCaret class
// Author:      Vadim Zeitlin (original code by Robert Roebling)
// Created:     25.05.99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CARET_H_
#define _WX_CARET_H_

#include "wx/timer.h"
#include "wx/dc.h"
#include "wx/overlay.h"

class WXDLLIMPEXP_FWD_CORE wxCaret;

class WXDLLIMPEXP_CORE wxCaretTimer : public wxTimer
{
public:
    explicit wxCaretTimer(wxCaret *caret);
    virtual void Notify() override;

private:
    wxCaret *m_caret;
};

class WXDLLIMPEXP_CORE wxCaret : public wxCaretBase
{
public:
    // ctors
    // -----
        // default - use Create()
    wxCaret() : m_timer(this) { }
        // creates a block caret associated with the given window
    wxCaret(wxWindowBase *window, int width, int height)
        : wxCaretBase(window, width, height), m_timer(this) { }
    wxCaret(wxWindowBase *window, const wxSize& size)
        : wxCaretBase(window, size), m_timer(this) { }

    virtual ~wxCaret();

    // implementation
    // --------------

    // called by wxWindow (not using the event tables)
    virtual void OnSetFocus() override;
    virtual void OnKillFocus() override;

    // we don't need to hide the caret if it's rendered using overlays
    virtual bool NeedsSuspend() const { return !m_overlay.IsNative(); }

    // called by wxCaretTimer
    void OnTimer();

protected:
    virtual void DoShow() override;
    virtual void DoHide() override;
    virtual void DoMove() override;
    virtual void DoSize() override;

    // blink the caret once
    void Blink();

    // refresh the caret
    void Refresh();

    // draw the caret on the given DC
    void DoDraw(wxDC *dc, wxWindow* win);

private:
    // the overlay for displaying the caret
    wxOverlay   m_overlay;
    // the bitmap holding the part of window hidden by the caret when it was
    // at (m_xOld, m_yOld)
    wxBitmap      m_bmpUnderCaret;
    int           m_xOld = -1,
                  m_yOld = -1;
    wxCaretTimer  m_timer;
    bool          m_blinkedOut = true,     // true => caret hidden right now
                  m_hasFocus = true;       // true => our window has focus
};

#endif // _WX_CARET_H_
