/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/infobar.cpp
// Purpose:     wxWinUI wxInfoBar implementation (WinUI InfoBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_INFOBAR

#include "wx/infobar.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/sizer.h"
    #include "wx/stockitem.h"
#endif

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

wxIMPLEMENT_DYNAMIC_CLASS(wxInfoBar, wxInfoBarBase);

class wxWinUIInfoBarImpl
{
public:
    wxWinUIControlHost host;
    MUXC::InfoBar bar{ nullptr };
    winrt::event_token closeToken{};
};

wxInfoBar::wxInfoBar()
{
}

wxInfoBar::wxInfoBar(wxWindow *parent, wxWindowID winid, long style)
{
    Create(parent, winid, style);
}

wxInfoBar::~wxInfoBar() = default;

bool wxInfoBar::Create(wxWindow *parent, wxWindowID winid, long style)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, winid, wxDefaultPosition, wxDefaultSize,
                            style | wxBORDER_NONE, wxDefaultValidator,
                            wxASCII_STR("infobar")) )
        return false;

    m_winui.reset(new wxWinUIInfoBarImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->bar = MUXC::InfoBar();
        m_winui->bar.IsOpen(false);
        m_winui->bar.IsClosable(true);

        m_winui->closeToken = m_winui->bar.CloseButtonClick(
            [this](MUXC::InfoBar const&, winrt::Windows::Foundation::IInspectable const&)
            {
                if ( m_winui )
                    Dismiss();
            });

        m_winui->host.SetContent(m_winui->bar);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar creation", e);
        return false;
    }

    // Like the other ports, the info bar starts hidden and is shown on demand.
    Hide();
    return true;
}

void wxInfoBar::ShowMessage(const wxString& msg, int flags)
{
    if ( !m_winui || !m_winui->bar )
        return;

    MUXC::InfoBarSeverity severity = MUXC::InfoBarSeverity::Informational;
    if ( flags & wxICON_ERROR )
        severity = MUXC::InfoBarSeverity::Error;
    else if ( flags & wxICON_WARNING )
        severity = MUXC::InfoBarSeverity::Warning;
    else if ( flags & wxICON_INFORMATION )
        severity = MUXC::InfoBarSeverity::Informational;

    try
    {
        m_winui->bar.Severity(severity);
        m_winui->bar.Message(wxWinUIToHString(msg));
        m_winui->bar.IsOpen(true);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar ShowMessage", e);
    }

    Show();
    UpdateParent();
    m_winui->host.ForceRender();
}

void wxInfoBar::Dismiss()
{
    if ( m_winui && m_winui->bar )
    {
        try
        {
            m_winui->bar.IsOpen(false);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    Hide();
    UpdateParent();
}

void wxInfoBar::AddButton(wxWindowID btnid, const wxString& label)
{
    ButtonInfo info;
    info.id = btnid;
    info.label = label.empty() ? wxGetStockLabel(btnid, wxSTOCK_NOFLAGS) : label;
    m_buttons.push_back(info);
    RebuildButtons();
}

void wxInfoBar::RemoveButton(wxWindowID btnid)
{
    for ( wxVector<ButtonInfo>::iterator it = m_buttons.begin();
          it != m_buttons.end();
          ++it )
    {
        if ( it->id == btnid )
        {
            m_buttons.erase(it);
            break;
        }
    }
    RebuildButtons();
}

size_t wxInfoBar::GetButtonCount() const
{
    return m_buttons.size();
}

wxWindowID wxInfoBar::GetButtonId(size_t idx) const
{
    wxCHECK_MSG( idx < m_buttons.size(), wxID_NONE, wxT("invalid infobar button index") );
    return m_buttons[idx].id;
}

bool wxInfoBar::HasButtonId(wxWindowID btnid) const
{
    for ( size_t i = 0; i < m_buttons.size(); ++i )
    {
        if ( m_buttons[i].id == btnid )
            return true;
    }
    return false;
}

wxSize wxInfoBar::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(200, 48), const_cast<wxInfoBar*>(this));
}

void wxInfoBar::RebuildButtons()
{
    if ( !m_winui || !m_winui->bar )
        return;

    try
    {
        if ( m_buttons.empty() )
        {
            // No custom buttons: show the built-in close (X) button.
            m_winui->bar.Content(nullptr);
            m_winui->bar.IsClosable(true);
            m_winui->host.ForceRender();
            return;
        }

        // Custom buttons replace the built-in close button.
        m_winui->bar.IsClosable(false);

        MUXC::StackPanel panel;
        panel.Orientation(winrt::Microsoft::UI::Xaml::Controls::Orientation::Horizontal);
        panel.Spacing(8);
        panel.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromLengths(0, 0, 0, 8));

        for ( size_t i = 0; i < m_buttons.size(); ++i )
        {
            const wxWindowID btnid = m_buttons[i].id;
            MUXC::Button button;
            MUXC::TextBlock text;
            text.Text(wxWinUIToHString(wxControl::GetLabelText(m_buttons[i].label)));
            button.Content(text);
            button.Click(
                [this, btnid](winrt::Windows::Foundation::IInspectable const&,
                              winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    if ( m_winui )
                        OnButtonClick(btnid);
                });
            panel.Children().Append(button);
        }

        m_winui->bar.Content(panel);
        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar RebuildButtons", e);
    }
}

void wxInfoBar::OnButtonClick(wxWindowID btnid)
{
    wxCommandEvent event(wxEVT_BUTTON, btnid);
    event.SetEventObject(this);

    // If the application does not handle the button, dismiss the bar (this
    // matches the generic info bar behavior).
    if ( !GetEventHandler()->ProcessEvent(event) )
        Dismiss();
}

void wxInfoBar::UpdateParent()
{
    if ( wxWindow *parent = GetParent() )
        parent->Layout();
}

#endif // wxUSE_INFOBAR
