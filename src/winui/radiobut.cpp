/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/radiobut.cpp
// Purpose:     wxWinUI wxRadioButton implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBTN

#include "wx/radiobut.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

class wxWinUIRadioButtonImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::RadioButton radioButton{ nullptr };
    winrt::event_token checkedToken{};
    bool updating = false;
};

wxRadioButton::wxRadioButton()
    : m_isChecked(false)
{
}

wxRadioButton::wxRadioButton(wxWindow *parent,
                             wxWindowID id,
                             const wxString& label,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
    : wxRadioButton()
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxRadioButton::~wxRadioButton() = default;

bool wxRadioButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIRadioButtonImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->radioButton = winrt::Microsoft::UI::Xaml::Controls::RadioButton();
        m_winui->checkedToken = m_winui->radioButton.Checked(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->updating )
                    return;

                m_isChecked = true;
                ClearRadioGroup();
                SendRadioEvent();
            });

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->radioButton);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton creation", e);
        return false;
    }

    if ( HasFlag(wxRB_GROUP) )
        SetValue(true);

    return true;
}

void wxRadioButton::SetValue(bool value)
{
    if ( m_isChecked == value )
    {
        UpdateWinUIContent();
        return;
    }

    m_isChecked = value;
    UpdateWinUIContent();

    if ( value && !HasFlag(wxRB_SINGLE) )
        ClearRadioGroup();
}

bool wxRadioButton::GetValue() const
{
    return m_isChecked;
}

void wxRadioButton::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    UpdateWinUIContent();
}

void wxRadioButton::Command(wxCommandEvent& event)
{
    SetValue(event.IsChecked());
    ProcessCommand(event);
}

wxSize wxRadioButton::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(140, 32), const_cast<wxRadioButton *>(this));
}

void wxRadioButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->radioButton )
        return;

    m_winui->updating = true;
    m_winui->radioButton.Foreground(wxWinUIBrush(32, 32, 32));

    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));
    textBlock.Foreground(wxWinUIBrush(32, 32, 32));
    m_winui->radioButton.Content(textBlock);
    m_winui->radioButton.IsChecked(m_isChecked);
    m_winui->updating = false;
}

void wxRadioButton::ClearRadioGroup()
{
    if ( !GetParent() || HasFlag(wxRB_SINGLE) )
        return;

    const wxWindowList& siblings = GetParent()->GetChildren();
    wxWindowList::compatibility_iterator nodeThis = siblings.Find(this);
    if ( !nodeThis )
        return;

    for ( wxWindowList::compatibility_iterator node = nodeThis->GetPrevious();
          node;
          node = node->GetPrevious() )
    {
        wxRadioButton *btn = wxDynamicCast(node->GetData(), wxRadioButton);
        if ( !btn )
            continue;
        if ( btn->HasFlag(wxRB_SINGLE) )
            break;

        btn->SetValue(false);
        if ( btn->HasFlag(wxRB_GROUP) )
            break;
    }

    for ( wxWindowList::compatibility_iterator node = nodeThis->GetNext();
          node;
          node = node->GetNext() )
    {
        wxRadioButton *btn = wxDynamicCast(node->GetData(), wxRadioButton);
        if ( !btn )
            continue;
        if ( btn->HasFlag(wxRB_GROUP | wxRB_SINGLE) )
            break;

        btn->SetValue(false);
    }
}

void wxRadioButton::SendRadioEvent()
{
    wxCommandEvent event(wxEVT_RADIOBUTTON, GetId());
    event.SetEventObject(this);
    event.SetInt(m_isChecked);
    ProcessCommand(event);
}

#endif // wxUSE_RADIOBTN
