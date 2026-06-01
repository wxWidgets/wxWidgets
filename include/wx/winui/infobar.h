/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/infobar.h
// Purpose:     wxWinUI wxInfoBar declaration (WinUI InfoBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_INFOBAR_H_
#define _WX_WINUI_INFOBAR_H_

#include "wx/vector.h"

#include <memory>

class wxWinUIInfoBarImpl;

class WXDLLIMPEXP_CORE wxInfoBar : public wxInfoBarBase
{
public:
    wxInfoBar();
    wxInfoBar(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);
    ~wxInfoBar() override;

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);

    void ShowMessage(const wxString& msg, int flags = wxICON_INFORMATION) override;
    void Dismiss() override;

    void AddButton(wxWindowID btnid, const wxString& label = wxString()) override;
    void RemoveButton(wxWindowID btnid) override;

    size_t GetButtonCount() const override;
    wxWindowID GetButtonId(size_t idx) const override;
    bool HasButtonId(wxWindowID btnid) const override;

protected:
    wxSize DoGetBestSize() const override;

    void RebuildButtons();
    void UpdateParent();
    void OnButtonClick(wxWindowID btnid);

    struct ButtonInfo
    {
        wxWindowID id;
        wxString label;
    };

    std::unique_ptr<wxWinUIInfoBarImpl> m_winui;
    wxVector<ButtonInfo> m_buttons;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxInfoBar);
};

#endif // _WX_WINUI_INFOBAR_H_
