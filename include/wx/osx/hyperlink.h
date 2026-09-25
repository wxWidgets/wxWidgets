///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/hyperlink.h
// Purpose:     wxHyperlinkCtrl for wxOSX
// Author:      Quin Gillespie
// Created:     2026-09-22
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_HYPERLINK_H_
#define _WX_OSX_HYPERLINK_H_

#include "wx/generic/hyperlink.h"

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxHyperlinkCtrl : public wxGenericHyperlinkCtrl
{
public:
    wxHyperlinkCtrl() = default;

    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label,
                    const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr))
    {
        Create(parent, id, label, url, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));

    virtual void SetVisited(bool visited = true) override;

    virtual bool SetForegroundColour(const wxColour& colour) override;

    virtual bool OSXHandleClicked(double timestampsec) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxHyperlinkCtrl);
};

#endif // _WX_OSX_HYPERLINK_H_
