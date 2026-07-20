/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/tglbtn.h
// Purpose:     wxWinUI wxToggleButton declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TGLBTN_H_
#define _WX_WINUI_TGLBTN_H_

#include "wx/bmpbndl.h"

#include <memory>

class wxWinUIToggleButtonImpl;

class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton();
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr));
    ~wxToggleButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    void SetValue(bool value) override;
    bool GetValue() const override;

    void SetLabel(const wxString& label) override;
    void Command(wxCommandEvent& event) override;

protected:
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    wxSize DoGetBestSize() const override;

    void SendToggleEvent();
    virtual void UpdateWinUIContent();

    std::unique_ptr<wxWinUIToggleButtonImpl> m_winui;
    bool m_state = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxToggleButton);
};

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapToggleButton : public wxToggleButton
{
public:
    wxBitmapToggleButton() = default;
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

    void SetLabel(const wxString& label) override
        { wxToggleButton::SetLabel(label); }

protected:
    wxSize DoGetBestSize() const override;
    void UpdateWinUIContent() override;
    void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;

private:
    wxBitmapBundle m_bitmap;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxBitmapToggleButton);
};

#endif // _WX_WINUI_TGLBTN_H_
