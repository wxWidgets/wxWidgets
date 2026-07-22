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

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Text.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

wxIMPLEMENT_DYNAMIC_CLASS(wxInfoBar, wxInfoBarBase);

namespace
{

MUXM::SolidColorBrush wxWinUIInfoBarBrush(const wxColour& col)
{
    return wxWinUIBrush(col.Red(), col.Green(), col.Blue(), col.Alpha());
}

void wxWinUIInfoBarApplyFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !font.IsOk() )
    {
        // an invalid font resets the window font: bring the peer back to
        // its theme typography instead of keeping the previous values
        control.ClearValue(MUXC::Control::FontFamilyProperty());
        control.ClearValue(MUXC::Control::FontSizeProperty());
        control.ClearValue(MUXC::Control::FontWeightProperty());
        control.ClearValue(MUXC::Control::FontStyleProperty());
        return;
    }

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        control.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));
    else
        control.ClearValue(MUXC::Control::FontFamilyProperty());

    const double pt = font.GetFractionalPointSize();
    control.FontSize(pt > 0.0 ? pt * 96.0 / 72.0 : 14.0);
    control.FontWeight(font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
                           ? winrt::Microsoft::UI::Text::FontWeights::Bold()
                           : winrt::Microsoft::UI::Text::FontWeights::Normal());
    control.FontStyle(font.GetStyle() == wxFONTSTYLE_NORMAL
                          ? winrt::Windows::UI::Text::FontStyle::Normal
                          : winrt::Windows::UI::Text::FontStyle::Italic);
}

// Depth-first search of the visual tree for a named template element.
MUX::FrameworkElement
wxWinUIFindElementByName(MUX::DependencyObject const& root,
                         winrt::hstring const& name)
{
    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        auto child = MUXM::VisualTreeHelper::GetChild(root, i);
        if ( auto element = child.try_as<MUX::FrameworkElement>() )
        {
            if ( element.Name() == name )
                return element;
        }
        if ( auto found = wxWinUIFindElementByName(child, name) )
            return found;
    }
    return nullptr;
}

} // anonymous namespace

class wxWinUIInfoBarImpl
{
public:
    wxWinUIControlHost host;
    MUXC::InfoBar bar{ nullptr };
    MUXC::CheckBox checkBox{ nullptr };
    winrt::event_token closingToken{};

    // Close-button state machine (see the Closing handler in Create()):
    // closeRequestPending is true while the wxID_CLOSE event of a native
    // close-button click is being processed, and dismissedDuringClose
    // records whether Dismiss() was called during that processing.
    bool closeRequestPending = false;
    bool dismissedDuringClose = false;
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
    // calling Hide() before Create() ensures that we're created initially
    // hidden
    Hide();
    if ( !wxControl::Create(parent, winid, wxDefaultPosition, wxDefaultSize,
                            style, wxDefaultValidator,
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

        // The native close (X) button must behave like the generic one: emit
        // wxEVT_BUTTON with wxID_CLOSE through the wx handler chain (the
        // official sample binds it to read the checkbox state) and dismiss
        // the bar unless the application handled the event and kept it open.
        //
        // When Closing fires, WinUI has already set IsOpen to false, and it
        // re-opens the bar itself after the callback if Cancel is true (see
        // OnCloseButtonClick()/RaiseClosingEvent() in microsoft-ui-xaml).
        // So the decision has to be made *inside* the callback: route the wx
        // event, let Dismiss() record its verdict through the state flags
        // (writing IsOpen here would not toggle anything), and cancel the
        // XAML closing only when the bar is meant to stay open.  A real
        // Dismiss() outside a click closes with Reason::Programmatic and
        // takes none of these branches.
        m_winui->closingToken = m_winui->bar.Closing(
            [this](MUXC::InfoBar const&, MUXC::InfoBarClosingEventArgs const& args)
            {
                if ( args.Reason() != MUXC::InfoBarCloseReason::CloseButton )
                    return;

                if ( !m_winui || m_winui->closeRequestPending )
                    return;

                m_winui->closeRequestPending = true;
                m_winui->dismissedDuringClose = false;
                OnButtonClick(wxID_CLOSE);
                if ( m_winui )
                {
                    m_winui->closeRequestPending = false;
                    if ( !m_winui->dismissedDuringClose )
                        args.Cancel(true);
                }
            });

        // Apply the appearance that was possibly set before creation.
        if ( m_hasFont )
            wxWinUIInfoBarApplyFont(m_winui->bar, GetFont());
        if ( m_hasFgCol )
            m_winui->bar.Foreground(wxWinUIInfoBarBrush(GetForegroundColour()));

        m_winui->host.SetContent(m_winui->bar);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar creation", e);
        return false;
    }

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

    const int icon = flags & wxICON_MASK;

    try
    {
        m_winui->bar.Severity(severity);
        m_winui->bar.IsIconVisible(icon != 0 && icon != wxICON_NONE);
        m_winui->bar.Message(wxWinUIToHString(msg));
        m_winui->bar.IsOpen(true);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar ShowMessage", e);
    }

    // the message changes the natural size of the bar
    InvalidateBestSize();

    if ( !IsShown() )
    {
        DoShow();
    }
    else // we're already shown
    {
        // just update the layout to correspond to the new message
        UpdateParent();
    }

    m_winui->host.ForceRender();
}

void wxInfoBar::Dismiss()
{
    if ( m_winui && m_winui->bar )
    {
        if ( m_winui->closeRequestPending )
        {
            // We are inside the Closing callback of a close-button click:
            // IsOpen is already false there, so writing it would not close
            // anything.  Record the decision instead; the callback then
            // lets the XAML closing complete without cancelling it.
            m_winui->dismissedDuringClose = true;
        }
        else
        {
            try
            {
                m_winui->bar.IsOpen(false);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }

    DoHide();
}

void wxInfoBar::DoShow()
{
    // re-layout the parent first so that the window expands into an already
    // unoccupied by the other controls area: for this we need to change our
    // internal visibility flag to force Layout() to take us into account (see
    // the identical dance in the generic implementation)

    // just change the internal flag indicating that the window is visible,
    // without really showing it
    wxWindowBase::Show();

    // adjust the parent layout to account for us
    UpdateParent();

    // reset the flag back before really showing the window or it wouldn't be
    // shown at all because it would believe itself already visible
    wxWindowBase::Show(false);

    // finally do really show the window: the effects are implemented with
    // AnimateWindow() on our HWND (see MSWShowWithEffect()); if that fails,
    // fall back to showing the bar without any effect rather than not
    // showing it at all
    const wxShowEffect effect = GetShowEffect();
    if ( effect == wxSHOW_EFFECT_NONE ||
            !ShowWithEffect(effect, GetEffectDuration()) )
    {
        // a failed ShowWithEffect() may have already toggled the wx
        // visibility flag before AnimateWindow() failed; reset it so that
        // the plain Show() below really shows the window instead of being
        // a no-op on an already-"shown" flag
        wxWindowBase::Show(false);
        Show();
    }
}

void wxInfoBar::DoHide()
{
    // same fallback policy (and visibility-flag reset) as in DoShow()
    const wxShowEffect effect = GetHideEffect();
    if ( effect == wxSHOW_EFFECT_NONE ||
            !HideWithEffect(effect, GetEffectDuration()) )
    {
        wxWindowBase::Show(true);
        Hide();
    }

    UpdateParent();
}

wxInfoBar::BarPlacement wxInfoBar::GetBarPlacement() const
{
    wxSizer * const sizer = GetContainingSizer();
    if ( !sizer )
        return BarPlacement_Unknown;

    const wxSizerItemList& siblings = sizer->GetChildren();
    if ( siblings.GetFirst()->GetData()->GetWindow() == this )
        return BarPlacement_Top;
    else if ( siblings.GetLast()->GetData()->GetWindow() == this )
        return BarPlacement_Bottom;
    else
        return BarPlacement_Unknown;
}

wxShowEffect wxInfoBar::GetShowEffect() const
{
    if ( m_showEffect != wxSHOW_EFFECT_MAX )
        return m_showEffect;

    switch ( GetBarPlacement() )
    {
        case BarPlacement_Top:
            return wxSHOW_EFFECT_SLIDE_TO_BOTTOM;

        case BarPlacement_Bottom:
            return wxSHOW_EFFECT_SLIDE_TO_TOP;

        default:
            wxFAIL_MSG( "unknown info bar placement" );
            wxFALLTHROUGH;

        case BarPlacement_Unknown:
            return wxSHOW_EFFECT_NONE;
    }
}

wxShowEffect wxInfoBar::GetHideEffect() const
{
    if ( m_hideEffect != wxSHOW_EFFECT_MAX )
        return m_hideEffect;

    switch ( GetBarPlacement() )
    {
        case BarPlacement_Top:
            return wxSHOW_EFFECT_SLIDE_TO_TOP;

        case BarPlacement_Bottom:
            return wxSHOW_EFFECT_SLIDE_TO_BOTTOM;

        default:
            wxFAIL_MSG( "unknown info bar placement" );
            wxFALLTHROUGH;

        case BarPlacement_Unknown:
            return wxSHOW_EFFECT_NONE;
    }
}

void wxInfoBar::ShowCheckBox(const wxString& checkBoxText, bool checked)
{
    wxASSERT_MSG( HasFlag(wxINFOBAR_CHECKBOX),
                  "wxINFOBAR_CHECKBOX style should be set if calling ShowCheckBox()!");

    m_checkBoxLabel = checkBoxText;
    m_checked = checked;

    RebuildContent();
}

bool wxInfoBar::SetFont(const wxFont& font)
{
    if ( !wxInfoBarBase::SetFont(font) )
        return false;

    // check that we're not called before Create()
    if ( m_winui && m_winui->bar )
    {
        try
        {
            wxWinUIInfoBarApplyFont(m_winui->bar, font);
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI InfoBar SetFont", e);
        }

        InvalidateBestSize();
    }

    return true;
}

bool wxInfoBar::SetForegroundColour(const wxColor& colour)
{
    if ( !wxInfoBarBase::SetForegroundColour(colour) )
        return false;

    if ( m_winui && m_winui->bar )
    {
        try
        {
            if ( colour.IsOk() )
                m_winui->bar.Foreground(wxWinUIInfoBarBrush(colour));
            else
                m_winui->bar.ClearValue(MUXC::Control::ForegroundProperty());
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI InfoBar SetForegroundColour", e);
        }
    }

    return true;
}

void wxInfoBar::AddButton(wxWindowID btnid, const wxString& label)
{
    ButtonInfo info;
    info.id = btnid;
    info.label = label.empty() ? wxGetStockLabel(btnid, wxSTOCK_NOFLAGS) : label;
    m_buttons.push_back(info);
    RebuildContent();
}

void wxInfoBar::RemoveButton(wxWindowID btnid)
{
    // remove the last-added button with this id, like the generic version
    // (ids should be unique, but if they are repeated removing the most
    // recently added one makes more sense)
    for ( size_t i = m_buttons.size(); i > 0; --i )
    {
        if ( m_buttons[i - 1].id == btnid )
        {
            m_buttons.erase(m_buttons.begin() + (i - 1));
            RebuildContent();
            return;
        }
    }

    wxFAIL_MSG( wxString::Format("button with id %d not found", btnid) );
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
    // Measure the real XAML content once it has been realized; before that,
    // fall back to a plausible default size.
    if ( m_winui && m_winui->host.IsContentLoaded() )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size.x > 0 && size.y > 0 )
            return size;
    }

    return wxWindow::FromDIP(wxSize(200, 48), const_cast<wxInfoBar*>(this));
}

void wxInfoBar::RebuildContent()
{
    if ( !m_winui || !m_winui->bar )
        return;

    try
    {
        m_winui->checkBox = nullptr;

        const bool hasButtons = !m_buttons.empty();
        const bool hasCheckBox = !m_checkBoxLabel.empty();

        // Custom buttons replace the built-in close (X) button.
        m_winui->bar.IsClosable(!hasButtons);

        if ( !hasButtons && !hasCheckBox )
        {
            m_winui->bar.Content(nullptr);
        }
        else
        {
            // InfoBar.Content renders under the message: stack the row of
            // custom buttons and the checkbox, like the generic version
            // lays out its second row.
            MUXC::StackPanel content;
            content.Orientation(MUXC::Orientation::Vertical);
            content.Spacing(8);
            content.Margin(MUX::ThicknessHelper::FromLengths(0, 0, 0, 8));

            if ( hasButtons )
            {
                MUXC::StackPanel buttonsPanel;
                buttonsPanel.Orientation(MUXC::Orientation::Horizontal);
                buttonsPanel.Spacing(8);

                for ( size_t i = 0; i < m_buttons.size(); ++i )
                {
                    const wxWindowID btnid = m_buttons[i].id;
                    MUXC::Button button;
                    MUXC::TextBlock text;
                    text.Text(wxWinUIToHString(wxControl::GetLabelText(m_buttons[i].label)));
                    button.Content(text);
                    button.Click(
                        [this, btnid](winrt::Windows::Foundation::IInspectable const&,
                                      MUX::RoutedEventArgs const&)
                        {
                            if ( m_winui )
                                OnButtonClick(btnid);
                        });
                    buttonsPanel.Children().Append(button);
                }

                content.Children().Append(buttonsPanel);
            }

            if ( hasCheckBox )
            {
                MUXC::CheckBox checkBox;
                checkBox.Content(winrt::box_value(
                    wxWinUIToHString(wxControl::GetLabelText(m_checkBoxLabel))));
                checkBox.IsChecked(m_checked);
                checkBox.Checked(
                    [this](winrt::Windows::Foundation::IInspectable const&,
                           MUX::RoutedEventArgs const&)
                    {
                        if ( m_winui )
                            m_checked = true;
                    });
                checkBox.Unchecked(
                    [this](winrt::Windows::Foundation::IInspectable const&,
                           MUX::RoutedEventArgs const&)
                    {
                        if ( m_winui )
                            m_checked = false;
                    });
                content.Children().Append(checkBox);
                m_winui->checkBox = checkBox;
            }

            m_winui->bar.Content(content);
        }

        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar RebuildContent", e);
    }

    // buttons and checkbox change the natural size of the bar
    InvalidateBestSize();
    UpdateParent();
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

bool wxInfoBar::WinUIIsPeerOpen() const
{
    if ( !m_winui || !m_winui->bar )
        return false;

    try
    {
        return m_winui->bar.IsOpen();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxInfoBar::WinUIClickCloseButton()
{
    if ( !m_winui || !m_winui->bar )
        return false;

    try
    {
        // "CloseButton" is the x:Name of the close button in the official
        // InfoBar control template.
        auto closeButton =
            wxWinUIFindElementByName(m_winui->bar, L"CloseButton")
                .try_as<MUXC::Button>();
        if ( !closeButton )
            return false;

        winrt::Microsoft::UI::Xaml::Automation::Peers::ButtonAutomationPeer
            peer(closeButton);
        peer.Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI InfoBar close-button invoke", e);
        return false;
    }
}

#endif // wxUSE_INFOBAR
