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

#include "wx/frame.h"

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTipWindow : public wxFrame
{
public:
    wxTipWindow(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100);

protected:
    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);

    // calculate the client rect we need to display the text
    void Adjust(const wxString& text, wxCoord maxLength);

private:
    wxArrayString m_textLines;
    wxCoord m_heightLine;

    DECLARE_EVENT_TABLE()
};

#endif // _WX_TIPWIN_H_
