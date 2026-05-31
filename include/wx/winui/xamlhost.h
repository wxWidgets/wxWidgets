/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/xamlhost.h
// Purpose:     wxWinUI XAML island host
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_XAMLHOST_H_
#define _WX_WINUI_XAMLHOST_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/window.h"

#include <memory>

class wxWinUIXamlHostImpl;

class WXDLLIMPEXP_CORE wxWinUIXamlHost : public wxWindow
{
public:
    wxWinUIXamlHost();
    wxWinUIXamlHost(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxBORDER_NONE,
                    const wxString& name = wxASCII_STR(wxPanelNameStr));
    ~wxWinUIXamlHost() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxBORDER_NONE,
                const wxString& name = wxASCII_STR(wxPanelNameStr));

    bool SetContentFromXaml(const wxString& xaml);
    void ClearContent();

private:
    bool InitializeXamlSource();
    void MoveAndResizeXamlSource();
    void OnSize(wxSizeEvent& event);

    std::unique_ptr<wxWinUIXamlHostImpl> m_impl;

    wxDECLARE_DYNAMIC_CLASS(wxWinUIXamlHost);
    wxDECLARE_NO_COPY_CLASS(wxWinUIXamlHost);
};

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_XAMLHOST_H_
