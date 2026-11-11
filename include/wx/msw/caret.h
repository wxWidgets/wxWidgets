///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/caret.h
// Purpose:     wxCaret class - the MSW implementation of wxCaret
// Author:      Vadim Zeitlin
// Created:     23.05.99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CARET_H_
#define _WX_CARET_H_

class WXDLLIMPEXP_CORE wxCaret : public wxCaretBase
{
public:
    wxCaret() = default;

        // create the caret of given (in pixels) width and height and associate
        // with the given window
    wxCaret(wxWindow *window, int width, int height)
    {
        (void)Create(window, width, height);
    }
        // same as above
    wxCaret(wxWindowBase *window, const wxSize& size)
    {
        (void)Create(window, size);
    }

    // process wxWindow notifications
    virtual void OnSetFocus() override;
    virtual void OnKillFocus() override;

protected:
    // override base class virtuals
    virtual void DoMove() override;
    virtual void DoShow() override;
    virtual void DoHide() override;
    virtual void DoSize() override;
    virtual void DoGetPosition(int* x, int* y) const override;

    // helper function which creates the system caret
    bool MSWCreateCaret();

private:
    bool m_hasCaret = false;

    wxDECLARE_NO_COPY_CLASS(wxCaret);
};

#endif // _WX_CARET_H_


