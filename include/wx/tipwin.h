///////////////////////////////////////////////////////////////////////////////
// Name:        wx/tipwin.h
// Purpose:     wxTipWindow is a window like the one typically used for
//              showing the tooltips
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIPWIN_H_
#define _WX_TIPWIN_H_

#ifdef __GNUG__
    #pragma interface "tipwin.h"
#endif

#if wxUSE_TIPWINDOW

#if wxUSE_POPUPWIN
#include "wx/popupwin.h"
#else
#include "wx/frame.h"
#endif

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

#if wxUSE_POPUPWIN
class WXDLLEXPORT wxTipWindow : public wxPopupTransientWindow
#else
class WXDLLEXPORT wxTipWindow : public wxFrame
#endif
{
    friend class wxTipWindowView;
public:
    // Supply windowPtr for it to null the given address
    // when the window has closed.
    wxTipWindow(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100, wxTipWindow** windowPtr = NULL);
    ~wxTipWindow();

    void SetTipWindowPtr(wxTipWindow** windowPtr) { m_windowPtr = windowPtr; }

    void Close();

protected:
    // event handlers
    void OnMouseClick(wxMouseEvent& event);
#if !wxUSE_POPUPWIN
    void OnActivate(wxActivateEvent& event);
    void OnKillFocus(wxFocusEvent& event);
#endif

private:
    wxArrayString m_textLines;
    wxCoord m_heightLine;
    wxTipWindow** m_windowPtr;

    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_TIPWINDOW
#endif // _WX_TIPWIN_H_
