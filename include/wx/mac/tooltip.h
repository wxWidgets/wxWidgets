///////////////////////////////////////////////////////////////////////////////
// Name:        mac/tooltip.h
// Purpose:     wxToolTip class - tooltip control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.01.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


//TO ADAPT...

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
    //void RelayEvent(WXMSG *msg);

private:
    // create the tooltip ctrl for our parent frame if it doesn't exist yet
    // and return its window handle
    WXHWND GetToolTipCtrl();

    // remove this tooltip from the tooltip control
    void Remove();

    wxString  m_text;           // tooltip text
    wxWindow *m_window;         // window we're associated with
};

