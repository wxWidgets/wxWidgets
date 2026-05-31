/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/button.cpp
// Purpose:     wxWinUI wxButton implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUTTON

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/stockitem.h"
#endif

#include "private.h"

#include <cmath>
#include <limits>

class wxWinUIButtonImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::Button button{ nullptr };
    winrt::event_token clickToken{};
};

wxButton::wxButton()
{
}

wxButton::wxButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxButton::~wxButton() = default;

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& labelOrig,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxString label;
    if ( !(style & wxBU_NOTEXT) )
    {
        label = labelOrig;
        if ( label.empty() && wxIsStockID(id) )
            label = wxGetStockLabel(id, wxSTOCK_WITH_MNEMONIC);
    }

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIButtonImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->button = winrt::Microsoft::UI::Xaml::Controls::Button();
        m_winui->clickToken = m_winui->button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                SendClickEvent();
            });

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->button);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button creation", e);
        return false;
    }

    return true;
}

void wxButton::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    UpdateWinUIContent();

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
        SetSize(GetBestSize());
}

void wxButton::Command(wxCommandEvent& event)
{
    ProcessCommand(event);
}

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    event.SetEventObject(this);
    return ProcessCommand(event);
}

wxSize wxButton::DoGetBestSize() const
{
    const wxSize defaultSize = GetDefaultSize(const_cast<wxButton *>(this));

    const wxString text = DontShowLabel()
                              ? wxString()
                              : wxControl::GetLabelText(GetLabel());
    if ( text.empty() )
        return defaultSize;

    // Measure the label with the actual WinUI font (wxGetTextExtent uses the
    // classic GDI font, which is narrower and would clip the text), then add
    // the WinUI Button content padding around it.
    wxSize best(0, 0);
    try
    {
        const float inf = std::numeric_limits<float>::infinity();
        winrt::Microsoft::UI::Xaml::Controls::TextBlock probe;
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

    best.x += FromDIP(28);
    best.y += FromDIP(14);

    best.IncTo(defaultSize);
    return best;
}

void wxButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button )
        return;

    const wxString text = DontShowLabel() ? wxString() : wxControl::GetLabelText(GetLabel());

    // Leave the brushes unset so the button follows the active WinUI theme.
    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(text));
    m_winui->button.Content(textBlock);
    m_winui->host.ForceRender();
}

wxSize wxButtonBase::GetDefaultSize(wxWindow *win)
{
    return wxWindow::FromDIP(wxSize(90, 32), win);
}

#endif // wxUSE_BUTTON
