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

    // Use the theme divider brush so the line follows light/dark mode; fall
    // back to a fixed grey if the resource is unavailable.
    const char *fmtTheme =
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        "Background=\"{ThemeResource DividerStrokeColorDefaultBrush}\" %s=\"1\" />";
    const char *fmtFixed =
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        "Background=\"#808080\" %s=\"1\" />";
    const char *dim = IsVertical() ? "Width" : "Height";

    for ( const char *fmt : { fmtTheme, fmtFixed } )
    {
        try
        {
            const auto loaded = winrt::Microsoft::UI::Xaml::Markup::XamlReader::Load(
                wxWinUIToHString(wxString::Format(fmt, dim)));
            m_winui->element = loaded.as<winrt::Microsoft::UI::Xaml::UIElement>();
            m_winui->host.SetContent(m_winui->element);
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI static line creation", e);
        }
    }
}

#endif // wxUSE_STATLINE
