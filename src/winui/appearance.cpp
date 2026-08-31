/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/appearance.cpp
// Purpose:     Common wx-to-WinUI appearance helpers
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/control.h"
#include "private.h"
#include "wx/winui/private/appearance.h"

#include <cstdint>
#include <cmath>
#include <limits>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Text.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

wxWinUIHighContrastOverrideForTesting gs_highContrastOverride =
    wxWinUIHighContrastOverrideForTesting::System;

struct wxWinUIAccessibleElementPeer
    : MUXAP::FrameworkElementAutomationPeerT<
          wxWinUIAccessibleElementPeer>
{
    wxWinUIAccessibleElementPeer(
        const MUX::FrameworkElement& owner,
        MUXAP::AutomationControlType controlType,
        const winrt::hstring& className)
        : FrameworkElementAutomationPeerT(owner),
          m_controlType(controlType),
          m_className(className)
    {
    }

    MUXAP::AutomationControlType GetAutomationControlTypeCore() const
    {
        return m_controlType;
    }

    winrt::hstring GetClassNameCore() const
    {
        return m_className;
    }

private:
    MUXAP::AutomationControlType m_controlType;
    winrt::hstring m_className;
};

struct wxWinUIAccessibleGrid
    : MUXC::GridT<wxWinUIAccessibleGrid>
{
    wxWinUIAccessibleGrid(MUXAP::AutomationControlType controlType,
                          const winrt::hstring& className)
        : m_controlType(controlType),
          m_className(className)
    {
    }

    MUXAP::AutomationPeer OnCreateAutomationPeer()
    {
        // GridT is the implementation object, not its projected Grid. Query
        // the composed inner object for the FrameworkElement projection that
        // FrameworkElementAutomationPeer requires as its owner.
        const MUX::FrameworkElement owner =
            this->try_as<MUX::FrameworkElement>();
        return winrt::make<wxWinUIAccessibleElementPeer>(
            owner,
            m_controlType,
            m_className);
    }

private:
    MUXAP::AutomationControlType m_controlType;
    winrt::hstring m_className;
};

template <typename T>
bool wxWinUIHasLocalValue(const T& object,
                          const MUX::DependencyProperty& property)
{
    return object &&
           object.ReadLocalValue(property) != MUX::DependencyProperty::UnsetValue();
}

winrt::Windows::UI::Text::FontWeight
wxWinUIFontWeight(const wxFont& font)
{
    winrt::Windows::UI::Text::FontWeight weight{};
    weight.Weight =
        static_cast<std::uint16_t>(font.GetNumericWeight());
    return weight;
}

winrt::Windows::UI::Text::FontStyle
wxWinUIFontStyle(const wxFont& font)
{
    switch ( font.GetStyle() )
    {
        case wxFONTSTYLE_ITALIC:
            return winrt::Windows::UI::Text::FontStyle::Italic;

        case wxFONTSTYLE_SLANT:
            return winrt::Windows::UI::Text::FontStyle::Oblique;

        case wxFONTSTYLE_NORMAL:
        default:
            return winrt::Windows::UI::Text::FontStyle::Normal;
    }
}

template <typename T>
void wxWinUIApplyFontImpl(const T& element,
                          const wxFont& font,
                          const MUX::DependencyProperty& familyProperty,
                          const MUX::DependencyProperty& sizeProperty,
                          const MUX::DependencyProperty& weightProperty,
                          const MUX::DependencyProperty& styleProperty)
{
    if ( !element )
        return;

    if ( !font.IsOk() )
    {
        element.ClearValue(familyProperty);
        element.ClearValue(sizeProperty);
        element.ClearValue(weightProperty);
        element.ClearValue(styleProperty);
        return;
    }

    const wxString face = font.GetFaceName();
    if ( face.empty() )
        element.ClearValue(familyProperty);
    else
        element.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));

    const double pointSize = font.GetFractionalPointSize();
    if ( pointSize > 0.0 )
        element.FontSize(pointSize * 96.0 / 72.0);
    else
        element.ClearValue(sizeProperty);

    element.FontWeight(wxWinUIFontWeight(font));
    element.FontStyle(wxWinUIFontStyle(font));
}

void wxWinUICaptureAccessibleValues(
    const MUX::UIElement& element,
    wxWinUIAppearanceSnapshot& snapshot)
{
    if ( !element )
        return;

    snapshot.automationName =
        wxString(MUXA::AutomationProperties::GetName(element).c_str());
    snapshot.accessKey =
        wxString(MUXA::AutomationProperties::GetAccessKey(element).c_str());
    snapshot.localizedControlType =
        wxString(
            MUXA::AutomationProperties::GetLocalizedControlType(element).c_str());

    const MUX::FrameworkElement frameworkElement =
        element.try_as<MUX::FrameworkElement>();
    if ( !frameworkElement )
        return;

    const MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            frameworkElement);
    if ( !peer )
        return;

    snapshot.hasAutomationPeer = true;
    snapshot.automationControlType =
        static_cast<int>(peer.GetAutomationControlType());
    snapshot.automationClassName =
        wxString(peer.GetClassName().c_str());
    snapshot.peerName = wxString(peer.GetName().c_str());
    snapshot.peerLocalizedControlType =
        wxString(peer.GetLocalizedControlType().c_str());
}

} // namespace

bool wxWinUIIsHighContrastActive()
{
    switch ( gs_highContrastOverride )
    {
        case wxWinUIHighContrastOverrideForTesting::ForceOff:
            return false;

        case wxWinUIHighContrastOverrideForTesting::ForceOn:
            return true;

        case wxWinUIHighContrastOverrideForTesting::System:
        {
            HIGHCONTRAST highContrast{};
            highContrast.cbSize = sizeof(highContrast);
            if ( !::SystemParametersInfo(
                    SPI_GETHIGHCONTRAST,
                    sizeof(highContrast),
                    &highContrast,
                    0) )
            {
                // This helper is called from peer projection transactions.
                // Logging the failure here could invoke an application log
                // target and synchronously destroy the projecting control.
                // Conservatively relinquish default local brushes so WinUI's
                // ThemeResources remain authoritative; custom colours are
                // still kept by their per-state provenance.
                return true;
            }
            return (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
        }
    }

    wxFAIL_MSG("unknown WinUI High Contrast override");
    return false;
}

void wxWinUISetHighContrastOverrideForTesting(
    wxWinUIHighContrastOverrideForTesting overrideValue)
{
    gs_highContrastOverride = overrideValue;
}

wxWinUILabelPresentation wxWinUIParseLabel(const wxString& label)
{
    wxWinUILabelPresentation result;
    const int mnemonic = wxControl::FindAccelIndex(label, &result.text);
    if ( mnemonic != wxNOT_FOUND &&
         static_cast<size_t>(mnemonic) < result.text.length() )
    {
        result.accessKey = result.text.Mid(mnemonic, 1);
    }

    return result;
}

MUXM::SolidColorBrush wxWinUICreateColourBrush(const wxColour& colour)
{
    return wxWinUIBrush(colour.Red(), colour.Green(), colour.Blue(),
                        colour.Alpha());
}

wxSize wxWinUIMeasureText(const wxWindow *window,
                          const wxString& text,
                          const wxFont& font)
{
    try
    {
        const float infinity = std::numeric_limits<float>::infinity();
        MUXC::TextBlock probe;
        probe.Text(wxWinUIToHString(text));
        wxWinUIApplyFont(probe, font);
        probe.Measure({ infinity, infinity });
        const auto desired = probe.DesiredSize();
        const wxSize dips(
            static_cast<int>(std::ceil(desired.Width)),
            static_cast<int>(std::ceil(desired.Height)));
        return window ? window->FromDIP(dips) : dips;
    }
    catch ( const winrt::hresult_error& )
    {
        return window ? window->GetTextExtent(text) : wxSize();
    }
}

void wxWinUIApplyFont(const MUXC::Control& control, const wxFont& font)
{
    wxWinUIApplyFontImpl(
        control, font,
        MUXC::Control::FontFamilyProperty(),
        MUXC::Control::FontSizeProperty(),
        MUXC::Control::FontWeightProperty(),
        MUXC::Control::FontStyleProperty());
}

void wxWinUIApplyFont(const MUXC::TextBlock& text, const wxFont& font)
{
    wxWinUIApplyFontImpl(
        text, font,
        MUXC::TextBlock::FontFamilyProperty(),
        MUXC::TextBlock::FontSizeProperty(),
        MUXC::TextBlock::FontWeightProperty(),
        MUXC::TextBlock::FontStyleProperty());
}

void wxWinUIApplyForeground(const MUXC::Control& control,
                            const wxColour& colour)
{
    if ( !control )
        return;

    if ( colour.IsOk() )
        control.Foreground(wxWinUICreateColourBrush(colour));
    else
        control.ClearValue(MUXC::Control::ForegroundProperty());
}

void wxWinUIApplyForeground(const MUXC::TextBlock& text,
                            const wxColour& colour)
{
    if ( !text )
        return;

    if ( colour.IsOk() )
        text.Foreground(wxWinUICreateColourBrush(colour));
    else
        text.ClearValue(MUXC::TextBlock::ForegroundProperty());
}

void wxWinUIApplyBackground(const MUXC::Control& control,
                            const wxColour& colour)
{
    if ( !control )
        return;

    if ( colour.IsOk() )
        control.Background(wxWinUICreateColourBrush(colour));
    else
        control.ClearValue(MUXC::Control::BackgroundProperty());
}

void wxWinUIApplyBackground(const MUXC::Border& border,
                            const wxColour& colour)
{
    if ( !border )
        return;

    if ( colour.IsOk() )
        border.Background(wxWinUICreateColourBrush(colour));
    else
        border.ClearValue(MUXC::Border::BackgroundProperty());
}

void wxWinUIApplyAccessKey(const MUX::UIElement& element,
                           const wxString& label)
{
    if ( !element )
        return;

    const wxWinUILabelPresentation presentation = wxWinUIParseLabel(label);
    const auto accessKey = wxWinUIToHString(presentation.accessKey);

    MUXA::AutomationProperties::SetAccessKey(element, accessKey);
    element.AccessKey(accessKey);
}

void wxWinUIApplyAccessibleLabel(const MUX::UIElement& element,
                                 const wxString& label,
                                 const wxString& localizedControlType)
{
    if ( !element )
        return;

    const wxWinUILabelPresentation presentation = wxWinUIParseLabel(label);
    MUXA::AutomationProperties::SetName(
        element, wxWinUIToHString(presentation.text));

    if ( localizedControlType.empty() )
    {
        element.ClearValue(
            MUXA::AutomationProperties::LocalizedControlTypeProperty());
    }
    else
    {
        MUXA::AutomationProperties::SetLocalizedControlType(
            element, wxWinUIToHString(localizedControlType));
    }
}

MUXC::Grid wxWinUICreateAccessibleGrid(
    MUXAP::AutomationControlType controlType,
    const wxString& className)
{
    MUXC::Grid root =
        winrt::make<wxWinUIAccessibleGrid>(
            controlType, wxWinUIToHString(className));
    MUXA::AutomationProperties::SetAutomationControlType(
        root, controlType);
    MUXA::AutomationProperties::SetAccessibilityView(
        root, MUXAP::AccessibilityView::Control);
    return root;
}

MUXC::Border
wxWinUICreateThemeBrushBorder(const wxString& resourceKey,
                              wxWinUIThemeBrushProperty property)
{
    const wxString propertyName =
        property == wxWinUIThemeBrushProperty::BorderBrush
            ? "BorderBrush"
            : "Background";
    const wxString xaml = wxString::Format(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        "%s=\"{ThemeResource %s}\" />",
        propertyName, resourceKey);

    try
    {
        return MUX::Markup::XamlReader::Load(wxWinUIToHString(xaml))
            .as<MUXC::Border>();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI theme brush lookup", e);
        return nullptr;
    }
}

wxWinUIAppearanceSnapshot
wxWinUICaptureAppearance(const MUXC::TextBlock& text,
                         const MUXC::Border& background,
                         const MUX::UIElement& accessibleElement)
{
    wxWinUIAppearanceSnapshot snapshot;
    if ( text )
    {
        snapshot.hasFontFamily =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontFamilyProperty());
        snapshot.hasFontSize =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontSizeProperty());
        snapshot.hasFontWeight =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontWeightProperty());
        snapshot.hasFontStyle =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontStyleProperty());
        snapshot.hasForeground =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::ForegroundProperty());
    }
    if ( background )
    {
        snapshot.hasBackground =
            wxWinUIHasLocalValue(background, MUXC::Border::BackgroundProperty());
    }
    wxWinUICaptureAccessibleValues(accessibleElement, snapshot);
    return snapshot;
}

wxWinUIAppearanceSnapshot
wxWinUICaptureAppearance(const MUXC::Control& control,
                         const MUX::UIElement& accessibleElement)
{
    wxWinUIAppearanceSnapshot snapshot;
    if ( control )
    {
        snapshot.hasFontFamily =
            wxWinUIHasLocalValue(control, MUXC::Control::FontFamilyProperty());
        snapshot.hasFontSize =
            wxWinUIHasLocalValue(control, MUXC::Control::FontSizeProperty());
        snapshot.hasFontWeight =
            wxWinUIHasLocalValue(control, MUXC::Control::FontWeightProperty());
        snapshot.hasFontStyle =
            wxWinUIHasLocalValue(control, MUXC::Control::FontStyleProperty());
        snapshot.hasForeground =
            wxWinUIHasLocalValue(control, MUXC::Control::ForegroundProperty());
        snapshot.hasBackground =
            wxWinUIHasLocalValue(control, MUXC::Control::BackgroundProperty());
    }
    wxWinUICaptureAccessibleValues(accessibleElement, snapshot);
    return snapshot;
}
