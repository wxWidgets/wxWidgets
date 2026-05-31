/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statline.cpp
// Purpose:     wxWinUI wxStaticLine implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATLINE

#include "wx/statline.h"

#include "private.h"

class wxWinUIStaticLineImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::UIElement element{ nullptr };
};

wxStaticLine::wxStaticLine()
{
}

wxStaticLine::wxStaticLine(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxStaticLine::~wxStaticLine() = default;

bool wxStaticLine::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    const wxSize adjustedSize = AdjustSize(size);
    if ( !wxControl::Create(parent, id, pos, adjustedSize, style, wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUIStaticLineImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    UpdateWinUIContent();
    return true;
}

void wxStaticLine::UpdateWinUIContent()
{
    if ( !m_winui )
        return;

    try
    {
        const char *xaml = IsVertical()
            ? "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
              "Background=\"#808080\" Width=\"1\" />"
            : "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
              "Background=\"#808080\" Height=\"1\" />";

        const auto loaded =
            winrt::Microsoft::UI::Xaml::Markup::XamlReader::Load(wxWinUIToHString(xaml));
        m_winui->element = loaded.as<winrt::Microsoft::UI::Xaml::UIElement>();
        m_winui->host.SetContent(m_winui->element);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI static line creation", e);
    }
}

#endif // wxUSE_STATLINE
