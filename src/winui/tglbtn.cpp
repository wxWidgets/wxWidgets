/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/tglbtn.cpp
// Purpose:     wxWinUI wxToggleButton implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#include "private.h"

#include <cmath>
#include <limits>

class wxWinUIToggleButtonImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton button{ nullptr };
    winrt::event_token clickToken{};
    bool updating = false;
};

wxDEFINE_EVENT( wxEVT_TOGGLEBUTTON, wxCommandEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxToggleButton);

wxToggleButton::wxToggleButton()
{
}

wxToggleButton::wxToggleButton(wxWindow *parent,
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

wxToggleButton::~wxToggleButton() = default;

bool wxToggleButton::Create(wxWindow *parent,
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

    m_winui.reset(new wxWinUIToggleButtonImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->button =
            winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton();
        m_winui->clickToken = m_winui->button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->updating )
                    return;

                if ( auto checked = m_winui->button.IsChecked() )
                    m_state = checked.Value();
                else
                    m_state = false;

                SendToggleEvent();
            });

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->button);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ToggleButton creation", e);
        return false;
    }

    return true;
}

void wxToggleButton::SetValue(bool value)
{
    m_state = value;
    if ( m_winui && m_winui->button )
    {
        m_winui->updating = true;
        m_winui->button.IsChecked(value);
        m_winui->updating = false;
        m_winui->host.ForceRender();
    }
}

bool wxToggleButton::GetValue() const
{
    return m_state;
}

void wxToggleButton::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    UpdateWinUIContent();
}

void wxToggleButton::Command(wxCommandEvent& event)
{
    SetValue(event.GetInt() != 0);
    ProcessCommand(event);
}

wxSize wxToggleButton::DoGetBestSize() const
{
    const wxSize defaultSize =
        wxButtonBase::GetDefaultSize(const_cast<wxToggleButton *>(this));

    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( text.empty() )
        return defaultSize;

    // Measure the label with the actual WinUI font (wxGetTextExtent uses the
    // narrower GDI font and would clip the text), then add the WinUI content
    // padding around it.
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

void wxToggleButton::SendToggleEvent()
{
    wxCommandEvent event(wxEVT_TOGGLEBUTTON, GetId());
    event.SetInt(m_state ? 1 : 0);
    event.SetEventObject(this);
    ProcessCommand(event);
}

void wxToggleButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button )
        return;

    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));
    m_winui->button.Content(textBlock);
    m_winui->host.ForceRender();
}

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------

bool wxBitmapToggleButton::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxBitmapBundle& label,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    // Store the bitmap first so that the base Create's UpdateWinUIContent
    // call (virtual) already renders it.
    m_bitmap = label;

    return wxToggleButton::Create(parent, id, wxString(), pos, size, style,
                                  validator, name);
}

void wxBitmapToggleButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    if ( which != State_Normal )
        return;

    m_bitmap = bitmap;
    UpdateWinUIContent();
    InvalidateBestSize();
}

wxSize wxBitmapToggleButton::DoGetBestSize() const
{
    if ( !m_bitmap.IsOk() )
        return wxToggleButton::DoGetBestSize();

    const wxSize bmp = m_bitmap.GetPreferredLogicalSizeFor(this);
    return wxSize(bmp.x + FromDIP(24), bmp.y + FromDIP(14));
}

void wxBitmapToggleButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button )
        return;

    if ( !m_bitmap.IsOk() )
    {
        wxToggleButton::UpdateWinUIContent();
        return;
    }

    try
    {
        const wxBitmap bmp = m_bitmap.GetBitmapFor(this);
        if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
        {
            winrt::Microsoft::UI::Xaml::Controls::Image image;
            image.Source(source);
            image.Width(bmp.GetLogicalWidth());
            image.Height(bmp.GetLogicalHeight());
            m_winui->button.Content(image);
        }
        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI BitmapToggleButton content", e);
    }
}

#endif // wxUSE_TOGGLEBTN
