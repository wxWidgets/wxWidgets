///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/statusbr.h
// Purpose:     native implementation of wxStatusBar.
//              Optional: can use generic version instead.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBAR_H_
#define _WX_STATBAR_H_

class WXDLLEXPORT wxStatusBarMac : public wxStatusBarGeneric
{
    DECLARE_DYNAMIC_CLASS(wxStatusBarMac)

    wxStatusBarMac();
    wxStatusBarMac(wxWindow *parent, wxWindowID id = wxID_ANY,
           long style = wxST_SIZEGRIP,
           const wxString& name = wxStatusBarNameStr);

    virtual ~wxStatusBarMac();

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
              long style = wxST_SIZEGRIP,
              const wxString& name = wxStatusBarNameStr);

    virtual void DrawFieldText(wxDC& dc, int i);
    virtual void DrawField(wxDC& dc, int i);
    virtual void SetStatusText(const wxString& text, int number = 0);

    // Implementation
    virtual void MacHiliteChanged();
    void OnPaint(wxPaintEvent& event);

protected:
    DECLARE_EVENT_TABLE()
};

#endif // _WX_STATBAR_H_
