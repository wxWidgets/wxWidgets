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

    virtual void SetPosition(int x, int y) wxOVERRIDE;

    // blink the caret
    void Blink();

    // draw the caret
    void Draw();

    void SetupTimer();

    void OnTimer(wxTimerEvent& event);

private:
    bool m_hasFocus;
    bool m_blinkedOut;

    wxTimer m_timer;

    wxDECLARE_NO_COPY_CLASS(wxCaret);
};

#endif // _WX_CARET_H_
