/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/tglbtn.h
// Purpose:     Declaration of the wxToggleButton class, which implements a
//              toggle button under wxMac.
// Author:      Stefan Csomor
// Created:     08.02.01
// Copyright:   (c) 2004 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOGGLEBUTTON_H_
#define _WX_TOGGLEBUTTON_H_

class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton() {}
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    virtual bool OSXHandleClicked( double timestampsec ) override;

    virtual void Command(wxCommandEvent& event) override;

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxToggleButton);
};


class WXDLLIMPEXP_CORE wxBitmapToggleButton : public wxToggleButton
{
public:
    wxBitmapToggleButton() {}
    wxBitmapToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxBitmapToggleButton);
};

#endif // _WX_TOGGLEBUTTON_H_

