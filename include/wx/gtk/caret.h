///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/caret.h
// Purpose:     wxCaret class - the GTK3 implementation of wxCaret
// Author:      Kettab Ali
// Created:     2022-01-29
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CARET_H_
#define _WX_CARET_H_

#include "wx/timer.h"

class WXDLLIMPEXP_CORE wxCaret : public wxCaretBase
{
public:
    wxCaret() { Init(); }
        // create the caret of given (in pixels) width and height and associate
        // with the given window
    wxCaret(wxWindow *window, int width, int height)
    {
        Init();

        (void)Create(window, width, height);

        SetupTimer();
    }
        // same as above
    wxCaret(wxWindowBase *window, const wxSize& size)
    {
        Init();

        (void)Create(window, size);

        SetupTimer();
    }

    // process wxWindow notifications
    virtual void OnSetFocus() wxOVERRIDE;
    virtual void OnKillFocus() wxOVERRIDE;

protected:
    void Init();

    // common part of On{Set,Kill}Focus()
    void OnFocus(bool hasFocus);

    // override base class virtuals
    virtual void DoMove() wxOVERRIDE;
    virtual void DoShow() wxOVERRIDE;
    virtual void DoHide() wxOVERRIDE;

    void Blink() { DoBlink(m_x, m_y); }
    void BlinkOld()
    {
        if ( m_xx != -1 || m_yy != -1 )
        {
            // blink out the caret at the old position.
            if ( !m_blinkedOut )
                DoBlink(m_xx, m_yy);
        }
    }

    // blink the caret at position (x,y)
    void DoBlink(int x, int y);

    // draw the caret at position (x,y)
    void Draw(int x, int y);

    void SetupTimer();

    void OnTimer(wxTimerEvent& event);

private:
    bool m_hasFocus;
    bool m_blinkedOut;

    int m_xx, m_yy; // old caret's position

    wxTimer m_timer;

    wxDECLARE_NO_COPY_CLASS(wxCaret);
};

#endif // _WX_CARET_H_
