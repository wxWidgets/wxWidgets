/////////////////////////////////////////////////////////////////////////////
// Name:        cshelp.h
// Purpose:     Context-sensitive help classes
// Author:      Julian Smart
// Modified by:
// Created:     08/09/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CSHELPH__
#define _WX_CSHELPH__

#ifdef __GNUG__
#pragma interface "cshelp.h"
#endif

#include "wx/defs.h"

#if wxUSE_HELP

#include "wx/bmpbuttn.h"

/*
 * wxContextHelp
 * Invokes context-sensitive help. When the user
 * clicks on a window, a wxEVT_HELP event will be sent to that
 * window for the application to display help for.
 */

class WXDLLEXPORT wxContextHelp: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxContextHelp)
public:
    wxContextHelp(wxWindow* win = NULL, bool beginHelp = TRUE);
    ~wxContextHelp();

    bool BeginContextHelp(wxWindow* win);
    bool EndContextHelp();

    bool EventLoop();
    bool DispatchEvent(wxWindow* win, const wxPoint& pt);

    void SetStatus(bool status) { m_status = status; }

protected:

    bool    m_inHelp;
    bool    m_status; // TRUE if the user left-clicked
};

/*
 * wxContextHelpButton
 * You can add this to your dialogs (especially on non-Windows platforms)
 * to put the application into context help mode.
 */

class WXDLLEXPORT wxContextHelpButton: public wxBitmapButton
{
public:
    wxContextHelpButton(wxWindow* parent, wxWindowID id = wxID_CONTEXT_HELP,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(20, -1),
        long style = wxBU_AUTODRAW);

    void OnContextHelp(wxCommandEvent& event);

    DECLARE_CLASS(wxContextHelpButton)
    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_HELP
#endif
// _WX_CSHELPH__
