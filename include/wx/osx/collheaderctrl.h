///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/collheaderctrl.h
// Purpose:     wxCollapsibleHeaderCtrl for wxOSX
// Author:      Quin Gillespie
// Created:     2026-09-22
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_COLLAPSIBLEHEADER_CTRL_H_
#define _WX_OSX_COLLAPSIBLEHEADER_CTRL_H_

class WXDLLIMPEXP_CORE wxCollapsibleHeaderCtrl
    : public wxCollapsibleHeaderCtrlBase
{
public:
    wxCollapsibleHeaderCtrl() = default;

    wxCollapsibleHeaderCtrl(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_NONE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxCollapsibleHeaderCtrlNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_NONE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxCollapsibleHeaderCtrlNameStr));

    virtual void SetCollapsed(bool collapsed = true) override;

    virtual bool IsCollapsed() const override;

    virtual bool OSXHandleClicked(double timestampsec) override;

private:
    wxDECLARE_NO_COPY_CLASS(wxCollapsibleHeaderCtrl);
};

#endif // _WX_OSX_COLLAPSIBLEHEADER_CTRL_H_
