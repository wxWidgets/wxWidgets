///////////////////////////////////////////////////////////////////////////////
// Name:        msw/tooltip.h
// Purpose:     wxToolTip class - tooltip control
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

class wxToolTip : public wxObject
{
public:
    // ctor & dtor
    wxToolTip(const wxString &rsTip);
    virtual ~wxToolTip();

    //
    // Accessors
    //
    inline const wxString& GetTip(void) const { return m_sText; }
    inline wxWindow*       GetWindow(void) const { return m_pWindow; }

           void            SetTip(const wxString& rsTip);
    inline void            SetWindow(wxWindow* pWin) { m_pWindow = pWin; }

    // controlling tooltip behaviour: globally change tooltip parameters
        // enable or disable the tooltips globally
    static void Enable(bool flag) {}
        // set the delay after which the tooltip appears
    static void SetDelay(long milliseconds) {}

    //
    // Implementation
    //
    void DisplayToolTipWindow(const wxPoint& rPos);
    void HideToolTipWindow(void);

private:
    void Create(const wxString &rsTip);

    HWND                            m_hWnd;
    wxString                        m_sText;           // tooltip text
    wxWindow*                       m_pWindow;         // window we're associated with
}; // end of CLASS wxToolTip

