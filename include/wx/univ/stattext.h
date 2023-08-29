/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/stattext.h
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_STATTEXT_H_
#define _WX_UNIV_STATTEXT_H_

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
        Create(parent, wxID_ANY, label, pos, size, 0, wxASCII_STR(wxStaticTextNameStr));
    }

    // full form
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxASCII_STR(wxStaticTextNameStr))
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
                const wxString &name = wxASCII_STR(wxStaticTextNameStr));

    // implementation only from now on

    virtual void SetLabel(const wxString& label) override;

    virtual bool IsFocused() const override { return false; }

protected:
    // draw the control
    virtual void DoDraw(wxControlRenderer *renderer) override;

    virtual void WXSetVisibleLabel(const wxString& str) override;
    virtual wxString WXGetVisibleLabel() const override;

    wxDECLARE_DYNAMIC_CLASS(wxStaticText);
};

#endif // _WX_UNIV_STATTEXT_H_
