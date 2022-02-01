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
    wxCaret() : m_timer(this) { Init(); }
        // create the caret of given (in pixels) width and height and associate
        // with the given window
    wxCaret(wxWindow *window, int width, int height) : m_timer(this)
    {
        Init();

        (void)Create(window, width, height);
    }
        // same as above
    wxCaret(wxWindowBase *window, const wxSize& size) : m_timer(this)
    {
        Init();

        (void)Create(window, size);
    }

    // process wxWindow notifications
    virtual void OnSetFocus() wxOVERRIDE;
    virtual void OnKillFocus() wxOVERRIDE;

protected:
    void Init()
    {
        wxCaretBase::Init();

        if ( GetBlinkTime() == 1200 )
        {
            // for MSW compatibility, map gtk-cursor-blink-time
            // default value '1200' to '500'.
            SetBlinkTime(500);
        }

        m_hasFocus = true;
        m_blinkedOut = true;

        m_xx = m_yy = -1;
    }

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

    // called by CaretTimer
    void OnTimer();

    class CaretTimer : public wxTimer
    {
    public:
        CaretTimer(wxCaret* caret) : m_caret(caret) { }

        virtual void Notify() wxOVERRIDE
        {
            m_caret->OnTimer();
        }

    private:
        wxCaret* m_caret;

    } m_timer;

private:
    bool m_hasFocus;
    bool m_blinkedOut;

    int m_xx, m_yy; // old caret's position

    wxDECLARE_NO_COPY_CLASS(wxCaret);
};

#endif // _WX_CARET_H_
