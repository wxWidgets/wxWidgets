/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/checkbox.cpp
// Purpose:     wxWinUI wxCheckBox implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"

#include "private.h"

class wxWinUICheckBoxImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::CheckBox checkBox{ nullptr };
    winrt::event_token checkedToken{};
    winrt::event_token uncheckedToken{};
    winrt::event_token indeterminateToken{};
    bool updating = false;
};

wxCheckBox::wxCheckBox()
    : m_state(wxCHK_UNCHECKED)
{
}

wxCheckBox::wxCheckBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& label,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : m_state(wxCHK_UNCHECKED)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxCheckBox::~wxCheckBox() = default;

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    WXValidateStyle(&style);

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUICheckBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->checkBox = winrt::Microsoft::UI::Xaml::Controls::CheckBox();
        m_winui->checkBox.IsThreeState(Is3State());

        auto handler =
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->updating )
                    return;

                auto isChecked = m_winui->checkBox.IsChecked();
                if ( !isChecked )
                    m_state = wxCHK_UNDETERMINED;
                else
                    m_state = isChecked.Value() ? wxCHK_CHECKED : wxCHK_UNCHECKED;

                SendCheckBoxEvent();
            };

        m_winui->checkedToken = m_winui->checkBox.Checked(handler);
        m_winui->uncheckedToken = m_winui->checkBox.Unchecked(handler);
        m_winui->indeterminateToken = m_winui->checkBox.Indeterminate(handler);

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->checkBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox creation", e);
        return false;
    }

    return true;
}

void wxCheckBox::SetValue(bool value)
{
    DoSet3StateValue(value ? wxCHK_CHECKED : wxCHK_UNCHECKED);
}

bool wxCheckBox::GetValue() const
{
    return m_state == wxCHK_CHECKED;
}

void wxCheckBox::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    UpdateWinUIContent();
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    SetValue(event.IsChecked());
    ProcessCommand(event);
}

wxSize wxCheckBox::DoGetBestClientSize() const
{
    return wxWindow::FromDIP(wxSize(140, 32), const_cast<wxCheckBox *>(this));
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState value)
{
    if ( value == wxCHK_UNDETERMINED && !Is3State() )
        value = wxCHK_UNCHECKED;

    m_state = value;
    UpdateWinUIContent();
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return m_state;
}

void wxCheckBox::SendCheckBoxEvent()
{
    wxCommandEvent event(wxEVT_CHECKBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(m_state == wxCHK_CHECKED);
    ProcessCommand(event);
}

void wxCheckBox::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->checkBox )
        return;

    m_winui->updating = true;
    m_winui->checkBox.Foreground(wxWinUIBrush(32, 32, 32));

    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));
    textBlock.Foreground(wxWinUIBrush(32, 32, 32));
    m_winui->checkBox.Content(textBlock);

    if ( m_state == wxCHK_UNDETERMINED )
        m_winui->checkBox.IsChecked(nullptr);
    else
        m_winui->checkBox.IsChecked(m_state == wxCHK_CHECKED);

    m_winui->updating = false;
}

#endif // wxUSE_CHECKBOX
