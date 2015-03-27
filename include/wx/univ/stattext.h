/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/stattext.h
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_UNIV_STATTEXT_H_
#define WX_UNIV_STATTEXT_H_

#include "wx/generic/stattextg.h"

class WXDLLIMPEXP_CORE wxStaticText : public wxGenericStaticText
{
public:
    wxStaticText() { }

    // usual ctor
    wxStaticText(wxWindow *parent,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize)
    {
        Create(parent, wxID_ANY, label, pos, size, 0, wxStaticTextNameStr);
    }

    // full form
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    // function ctor
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxStaticTextNameStr);

    // implementation only from now on

    virtual void SetLabel(const wxString& label) wxOVERRIDE;

    virtual bool IsFocused() const wxOVERRIDE { return false; }

protected:
    // draw the control
    virtual void DoDraw(wxControlRenderer *renderer) wxOVERRIDE;

    virtual void DoSetLabel(const wxString& str) wxOVERRIDE;
    virtual wxString DoGetLabel() const wxOVERRIDE;

    DECLARE_DYNAMIC_CLASS(wxStaticText)
};

#endif // _WX_UNIV_STATTEXT_H_
