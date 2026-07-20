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
    #include "wx/font.h"
#endif

#include "private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Windows.UI.Text.h>

#include <cmath>
#include <limits>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

// Apply a wxFont (family/size/weight/style) to a WinUI Control; no-op for an
// invalid font so default controls keep the native WinUI font.
void wxWinUIApplyControlFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !control || !font.IsOk() )
        return;

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        control.FontFamily(
            winrt::Microsoft::UI::Xaml::Media::FontFamily(wxWinUIToHString(face)));

    const double pt = font.GetFractionalPointSize();
    control.FontSize(pt > 0.0 ? pt * 96.0 / 72.0 : 14.0);
    control.FontWeight(font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
        ? winrt::Microsoft::UI::Text::FontWeights::Bold()
        : winrt::Microsoft::UI::Text::FontWeights::Normal());
    control.FontStyle(font.GetStyle() == wxFONTSTYLE_NORMAL
        ? winrt::Windows::UI::Text::FontStyle::Normal
        : winrt::Windows::UI::Text::FontStyle::Italic);
}

} // namespace

class wxWinUIRadioButtonImpl
{
public:
    wxWinUIControlHost host;
    MUXC::RadioButton radioButton{ nullptr };
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
        m_winui->radioButton = MUXC::RadioButton();
        m_winui->checkedToken = m_winui->radioButton.Checked(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->updating )
                    return;

                m_isChecked = true;
                ClearRadioGroup();
                SendRadioEvent();
            });

        UpdateWinUIContent();
        UpdateWinUIAppearance();
        ApplyToolTip();
        m_winui->host.SetContent(m_winui->radioButton);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton creation", e);
        return false;
    }

    SetInitialSize(size);

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
    InvalidateBestSize();
    UpdateWinUIContent();

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
        SetSize(GetBestSize());
}

void wxRadioButton::Command(wxCommandEvent& event)
{
    SetValue(event.IsChecked());
    ProcessCommand(event);
}

bool wxRadioButton::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return rc;
}

bool wxRadioButton::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxRadioButton::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

#if wxUSE_TOOLTIPS
void wxRadioButton::DoSetToolTipText(const wxString& tip)
{
    m_tooltipText = tip;
    ApplyToolTip();
}

void wxRadioButton::DoSetToolTip(wxToolTip *tip)
{
    m_tooltipText = tip ? tip->GetTip() : wxString();
    delete tip;
    ApplyToolTip();
}
#endif // wxUSE_TOOLTIPS

void wxRadioButton::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( m_winui && m_winui->radioButton )
    {
        m_winui->radioButton.IsEnabled(enable);
        m_winui->host.ForceRender();
    }
}

wxSize wxRadioButton::DoGetBestSize() const
{
    // Measure the actual WinUI radio button (glyph + label + padding).
    if ( m_winui )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size != wxDefaultSize )
            return size;
    }

    // Not realised yet: fall back to a guess big enough for the label, which
    // will be corrected once the content is loaded.
    wxSize best = wxWindow::FromDIP(wxSize(32, 32), const_cast<wxRadioButton *>(this));
    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( !text.empty() )
        best.x += GetTextExtent(text).x;

    return best;
}

void wxRadioButton::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->radioButton )
        return;

    m_winui->updating = true;

    try
    {
        MUXC::TextBlock textBlock;
        textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));

        if ( HasFlag(wxALIGN_RIGHT) )
        {
            m_winui->radioButton.FlowDirection(MUX::FlowDirection::RightToLeft);
            textBlock.FlowDirection(MUX::FlowDirection::LeftToRight);
        }
        else
        {
            m_winui->radioButton.FlowDirection(MUX::FlowDirection::LeftToRight);
        }

        m_winui->radioButton.Content(textBlock);
        m_winui->radioButton.IsChecked(m_isChecked);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton content", e);
    }

    m_winui->updating = false;
    m_winui->host.ForceRender();
}

void wxRadioButton::UpdateWinUIAppearance()
{
    if ( !m_winui || !m_winui->radioButton )
        return;

    try
    {
        if ( m_hasFont )
            wxWinUIApplyControlFont(m_winui->radioButton, GetFont());
        else
            m_winui->radioButton.ClearValue(MUXC::Control::FontSizeProperty());

        if ( UseForegroundColour() )
        {
            const wxColour& c = GetForegroundColour();
            m_winui->radioButton.Foreground(
                wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            m_winui->radioButton.ClearValue(MUXC::Control::ForegroundProperty());
        }

        if ( UseBackgroundColour() )
        {
            const wxColour& c = GetBackgroundColour();
            m_winui->radioButton.Background(
                wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            m_winui->radioButton.ClearValue(MUXC::Control::BackgroundProperty());
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButton appearance", e);
    }

    m_winui->host.ForceRender();
}

void wxRadioButton::ApplyToolTip()
{
#if wxUSE_TOOLTIPS
    if ( m_winui && m_winui->radioButton )
        wxWinUISetToolTip(m_winui->radioButton, m_tooltipText);
#endif // wxUSE_TOOLTIPS
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
