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

#include "wx/popupwin.h"

#if wxUSE_POPUPWIN

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTipWindow : public wxPopupTransientWindow
{
public:
    // Supply windowPtr for it to null the given address
    // when the window has closed.
    wxTipWindow(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100, wxTipWindow** windowPtr = NULL);
    ~wxTipWindow();

    void SetTipWindowPtr(wxTipWindow** windowPtr) { m_windowPtr = windowPtr; }

    // calculate the client rect we need to display the text
    void Adjust(const wxString& text, wxCoord maxLength);

    void Close();

protected:
    // event handlers
    void OnMouseClick(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);

private:
    wxArrayString m_textLines;
    wxCoord m_heightLine;
    wxTipWindow** m_windowPtr;

    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_POPUPWIN

#endif // _WX_TIPWIN_H_

