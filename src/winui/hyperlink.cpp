/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hyperlink.cpp
// Purpose:     wxWinUI wxHyperlinkCtrl implementation (WinUI HyperlinkButton)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/hyperlink.h"

#include "private.h"

#include <cmath>
#include <limits>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

class wxWinUIHyperlinkImpl
{
public:
    wxWinUIControlHost host;
    MUXC::HyperlinkButton button{ nullptr };
    winrt::event_token clickToken{};
};

wxHyperlinkCtrl::wxHyperlinkCtrl()
{
}

wxHyperlinkCtrl::wxHyperlinkCtrl(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& label,
                                 const wxString& url,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    Create(parent, id, label, url, pos, size, style, name);
}

wxHyperlinkCtrl::~wxHyperlinkCtrl() = default;

bool wxHyperlinkCtrl::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxString& label,
                             const wxString& url,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    CheckParams(label, url, style);

    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_url = url;
    wxControl::SetLabel(label.empty() ? url : label);

    m_winui.reset(new wxWinUIHyperlinkImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->button = MUXC::HyperlinkButton();
        m_winui->clickToken = m_winui->button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                SetVisited(true);
                SendEvent();
            });

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->button);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI HyperlinkButton creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxHyperlinkCtrl::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    UpdateWinUIContent();

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
}

wxSize wxHyperlinkCtrl::DoGetBestSize() const
{
    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( text.empty() )
        return wxControl::DoGetBestSize();

    wxSize best(0, 0);
    try
    {
        const float inf = std::numeric_limits<float>::infinity();
        MUXC::TextBlock probe;
        probe.Text(wxWinUIToHString(text));
        probe.Measure({ inf, inf });
        const auto desired = probe.DesiredSize();
        best.x = static_cast<int>(std::ceil(desired.Width));
        best.y = static_cast<int>(std::ceil(desired.Height));
    }
    catch ( const winrt::hresult_error& )
    {
        best = GetTextExtent(text);
    }

    // HyperlinkButton padding.
    best.x += FromDIP(22);
    best.y += FromDIP(12);
    return best;
}

void wxHyperlinkCtrl::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button )
        return;

    MUXC::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));

    // Apply the wx link colours when set; otherwise keep the theme default.
    const wxColour& c = m_visited && m_visitedColour.IsOk()
        ? m_visitedColour
        : m_normalColour;
    if ( c.IsOk() )
        textBlock.Foreground(wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));

    m_winui->button.Content(textBlock);
    m_winui->host.ForceRender();
}

#endif // wxUSE_HYPERLINKCTRL
