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

#ifndef WX_PRECOMP
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

class wxWinUICheckBoxImpl
{
public:
    wxWinUIControlHost host;
    MUXC::CheckBox checkBox{ nullptr };
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
        m_winui->checkBox = MUXC::CheckBox();

        // The WinUI check box cycles through the indeterminate state on click
        // only when IsThreeState is set; this must therefore only be enabled
        // for wxCHK_3STATE check boxes that also allow the user to set the 3rd
        // state.  A program-only 3rd state is still shown when set explicitly.
        m_winui->checkBox.IsThreeState(Is3State() && Is3rdStateAllowedForUser());

        auto handler =
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
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
        UpdateWinUIAppearance();
        ApplyToolTip();
        m_winui->host.SetContent(m_winui->checkBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox creation", e);
        return false;
    }

    SetInitialSize(size);

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
    InvalidateBestSize();
    UpdateWinUIContent();

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
        SetSize(GetBestSize());
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    SetValue(event.IsChecked());
    ProcessCommand(event);
}

bool wxCheckBox::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return rc;
}

bool wxCheckBox::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxCheckBox::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

#if wxUSE_TOOLTIPS
void wxCheckBox::DoSetToolTipText(const wxString& tip)
{
    m_tooltipText = tip;
    ApplyToolTip();
}

void wxCheckBox::DoSetToolTip(wxToolTip *tip)
{
    m_tooltipText = tip ? tip->GetTip() : wxString();
    delete tip;
    ApplyToolTip();
}
#endif // wxUSE_TOOLTIPS

void wxCheckBox::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( m_winui && m_winui->checkBox )
    {
        m_winui->checkBox.IsEnabled(enable);
        m_winui->host.ForceRender();
    }
}

wxSize wxCheckBox::DoGetBestClientSize() const
{
    // Measure the actual WinUI check box (glyph + label + padding) so the size
    // matches the real rendering rather than a hard-coded guess.
    if ( m_winui )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size != wxDefaultSize )
            return size;
    }

    // Not realised yet: fall back to a guess big enough for the label, which
    // will be corrected once the content is loaded.
    wxSize best = wxWindow::FromDIP(wxSize(32, 32), const_cast<wxCheckBox *>(this));
    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( !text.empty() )
        best.x += GetTextExtent(text).x;

    return best;
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

    try
    {
        MUXC::TextBlock textBlock;
        textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(GetLabel())));

        // wxALIGN_RIGHT puts the label on the *left* of the box.  WinUI always
        // lays the content out to the right of the box, so flip the control's
        // flow direction and flip the text back so it still reads left-to-right.
        if ( HasFlag(wxALIGN_RIGHT) )
        {
            m_winui->checkBox.FlowDirection(MUX::FlowDirection::RightToLeft);
            textBlock.FlowDirection(MUX::FlowDirection::LeftToRight);
        }
        else
        {
            m_winui->checkBox.FlowDirection(MUX::FlowDirection::LeftToRight);
        }

        m_winui->checkBox.Content(textBlock);

        if ( m_state == wxCHK_UNDETERMINED )
            m_winui->checkBox.IsChecked(nullptr);
        else
            m_winui->checkBox.IsChecked(m_state == wxCHK_CHECKED);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox content", e);
    }

    m_winui->updating = false;
    m_winui->host.ForceRender();
}

void wxCheckBox::UpdateWinUIAppearance()
{
    if ( !m_winui || !m_winui->checkBox )
        return;

    try
    {
        // Only override the font when the user set one, so the default check box
        // keeps the native WinUI font/metrics.
        if ( m_hasFont )
            wxWinUIApplyControlFont(m_winui->checkBox, GetFont());
        else
            m_winui->checkBox.ClearValue(MUXC::Control::FontSizeProperty());

        if ( UseForegroundColour() )
        {
            const wxColour& c = GetForegroundColour();
            m_winui->checkBox.Foreground(
                wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            m_winui->checkBox.ClearValue(MUXC::Control::ForegroundProperty());
        }

        if ( UseBackgroundColour() )
        {
            const wxColour& c = GetBackgroundColour();
            m_winui->checkBox.Background(
                wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            m_winui->checkBox.ClearValue(MUXC::Control::BackgroundProperty());
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckBox appearance", e);
    }

    m_winui->host.ForceRender();
}

void wxCheckBox::ApplyToolTip()
{
#if wxUSE_TOOLTIPS
    if ( m_winui && m_winui->checkBox )
        wxWinUISetToolTip(m_winui->checkBox, m_tooltipText);
#endif // wxUSE_TOOLTIPS
}

#endif // wxUSE_CHECKBOX
