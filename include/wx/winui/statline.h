/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/statline.h
// Purpose:     wxWinUI wxStaticLine declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATLINE_H_
#define _WX_WINUI_STATLINE_H_

#include <memory>

class wxWinUIStaticLineImpl;

class WXDLLIMPEXP_CORE wxStaticLine : public wxStaticLineBase
{
public:
    wxStaticLine();
    wxStaticLine(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString& name = wxASCII_STR(wxStaticLineNameStr));
    ~wxStaticLine() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLI_HORIZONTAL,
                const wxString& name = wxASCII_STR(wxStaticLineNameStr));

private:
    void UpdateWinUIContent();

    std::unique_ptr<wxWinUIStaticLineImpl> m_winui;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticLine);
};

#endif // _WX_WINUI_STATLINE_H_
