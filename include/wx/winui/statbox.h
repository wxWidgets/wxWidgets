/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/statbox.h
// Purpose:     wxWinUI wxStaticBox declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATBOX_H_
#define _WX_WINUI_STATBOX_H_

#include "wx/compositewin.h"

#include <memory>

class wxWinUIStaticBoxImpl;

class WXDLLIMPEXP_CORE wxStaticBox
    : public wxCompositeWindowSettersOnly<wxStaticBoxBase>
{
public:
    wxStaticBox();

    wxStaticBox(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    wxStaticBox(wxWindow *parent,
                wxWindowID id,
                wxWindow *label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    ~wxStaticBox() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                wxWindow *label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    void SetLabel(const wxString& label) override;
    bool SetBackgroundColour(const wxColour& colour) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetFont(const wxFont& font) override;
    void GetBordersForSizer(int *borderTop, int *borderOther) const override;

protected:
    wxSize DoGetBestSize() const override;
    wxWindowList GetCompositeWindowParts() const override;

private:
    void UpdateWinUIContent();
    void PositionLabelWindow();

    std::unique_ptr<wxWinUIStaticBoxImpl> m_winui;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticBox);
};

#define wxHAS_WINDOW_LABEL_IN_STATIC_BOX

#endif // _WX_WINUI_STATBOX_H_
