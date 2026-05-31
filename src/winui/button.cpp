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
    UpdateWinUIContent();
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
    return GetDefaultSize(const_cast<wxButton *>(this));
}

void wxButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button )
        return;

    const wxString text = DontShowLabel() ? wxString() : wxControl::GetLabelText(GetLabel());
    m_winui->button.Background(wxWinUIBrush(248, 248, 248));
    m_winui->button.BorderBrush(wxWinUIBrush(128, 128, 128));
    m_winui->button.Foreground(wxWinUIBrush(32, 32, 32));

    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(text));
    textBlock.Foreground(wxWinUIBrush(32, 32, 32));
    m_winui->button.Content(textBlock);
}

wxSize wxButtonBase::GetDefaultSize(wxWindow *win)
{
    return wxWindow::FromDIP(wxSize(90, 32), win);
}

#endif // wxUSE_BUTTON
