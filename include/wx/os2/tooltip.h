///////////////////////////////////////////////////////////////////////////////
// Name:        msw/tooltip.h
// Purpose:     wxToolTip class - tooltip control
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

class wxToolTip : public wxObject
{
public:
    // ctor & dtor
    wxToolTip(const wxString &tip);
    virtual ~wxToolTip();

    // accessors
        // tip text
    void SetTip(const wxString& tip);
    const wxString& GetTip() const { return m_text; }

        // the window we're associated with
    void SetWindow(wxWindow *win);
    wxWindow *GetWindow() const { return m_window; }

    // controlling tooltip behaviour: globally change tooltip parameters
        // enable or disable the tooltips globally
    static void Enable(bool flag);
        // set the delay after which the tooltip appears
    static void SetDelay(long milliseconds);

    // implementation
    void RelayEvent(WXMSG *msg);

private:
    static WXHWND hwndTT;
    // create the tooltip ctrl for our parent frame if it doesn't exist yet
    // and return its window handle
    WXHWND GetToolTipCtrl();

    // remove this tooltip from the tooltip control
    void Remove();

    wxString  m_text;           // tooltip text
    wxWindow *m_window;         // window we're associated with
};

