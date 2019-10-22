/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/statbox.h
// Purpose:     wxStaticBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBOX_H_
#define _WX_STATBOX_H_

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
    wxDECLARE_DYNAMIC_CLASS(wxStaticBox);

public:
    wxStaticBox() {}
    wxStaticBox(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxStaticBoxNameStr))
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    virtual void Command(wxCommandEvent& WXUNUSED(event)) wxOVERRIDE {}
    virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {}

    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const wxOVERRIDE;

    virtual bool AcceptsFocus() const wxOVERRIDE { return false; }

    // protect native font of box
    virtual bool SetFont( const wxFont &font ) wxOVERRIDE;
};

#endif
    // _WX_STATBOX_H_
